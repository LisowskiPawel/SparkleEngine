#include "D3D12/Resources/D3D12ResourceService.h"

#include "D3D12/D3D12TypeConversions.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Memory/D3D12GpuAllocation.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"
#include "RHI/Public/Diagnostics/RhiDiagnostics.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Validation/RhiContract.h"

#include <algorithm>
#include <cstring>
#include <d3d12.h>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogD3D12Diagnostics, "RHI.D3D12.Diagnostics");

D3D12ResourceService::D3D12ResourceService(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator, const RhiCapabilities& capabilities) noexcept :
    m_rhi(rhi),
    m_memoryAllocator(memoryAllocator),
    m_capabilities(capabilities)
{
}

RhiOwnedResourceHandle D3D12ResourceService::CreateTextureResource(
    const RhiTextureResourceDesc& desc,
    ResourceState initialState,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName)
{
	if (!RhiContract::IsTextureResourceDescUsable(m_capabilities, desc))
	{
		return {};
	}

	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildTextureResourceDesc(desc);
	const D3D12_RESOURCE_STATES resourceState = D3D12TypeConversions::ToResourceStates(initialState);
	const std::wstring_view resourceDebugName = debugName.empty() ? L"TextureResource" : debugName;

	std::unique_ptr<D3D12GpuAllocationRecord> ownedRecord = m_memoryAllocator.CreateTexture(resourceDesc, resourceState, nullptr, category, residencyClass, resourceDebugName);

	return MakeD3D12OwnedResourceHandle(std::move(ownedRecord));
}

RhiOwnedResourceHandle D3D12ResourceService::CreateBufferResource(
    const RhiBufferResourceDesc& desc,
    ResourceState initialState,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName)
{
	if (desc.SizeInBytes == 0)
	{
		return {};
	}

	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildBufferResourceDesc(desc);
	const D3D12_RESOURCE_STATES resourceState = residencyClass == RhiMemoryResidencyClass::HostUpload ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12TypeConversions::ToResourceStates(initialState);
	const std::wstring_view resourceDebugName = debugName.empty() ? L"BufferResource" : debugName;

	std::unique_ptr<D3D12GpuAllocationRecord> ownedRecord = m_memoryAllocator.CreateBuffer(resourceDesc, resourceState, category, residencyClass, resourceDebugName);
	if (ownedRecord == nullptr && residencyClass == RhiMemoryResidencyClass::HostUpload)
	{
		CollectCrashDiagnosticsOnce();
	}

	return MakeD3D12OwnedResourceHandle(std::move(ownedRecord));
}

bool D3D12ResourceService::WriteBufferResource(RhiOwnedResourceHandle resource, std::size_t destinationOffsetInBytes, const void* data, std::size_t sizeInBytes) noexcept
{
	D3D12GpuAllocationRecord* const record = GetD3D12GpuAllocationRecord(resource);
	if (record == nullptr || record->Resource == nullptr || data == nullptr || sizeInBytes == 0)
	{
		return false;
	}
	const D3D12_RESOURCE_DESC resourceDesc = record->Resource->GetDesc();
	const bool bBufferWriteInRange = destinationOffsetInBytes <= resourceDesc.Width && sizeInBytes <= resourceDesc.Width - destinationOffsetInBytes;

	if (!bBufferWriteInRange)
	{
		return false;
	}

	void* mappedData = nullptr;
	const D3D12_RANGE readRange{0, 0};
	if (FAILED(record->Resource->Map(0, &readRange, &mappedData)))
	{
		return false;
	}

	record->bIsMapped = true;
	record->CpuMappedAddress = mappedData;
	std::memcpy(static_cast<std::byte*>(mappedData) + destinationOffsetInBytes, data, sizeInBytes);
	const D3D12_RANGE writtenRange{destinationOffsetInBytes, destinationOffsetInBytes + sizeInBytes};
	record->Resource->Unmap(0, &writtenRange);
	record->bIsMapped = false;
	record->CpuMappedAddress = nullptr;
	return true;
}

void D3D12ResourceService::ReleaseOwnedResource(RhiOwnedResourceHandle resource) noexcept
{
	std::unique_ptr<D3D12GpuAllocationRecord> ownedRecord = TakeD3D12OwnedResourceHandle(resource);
	if (ownedRecord == nullptr)
	{
		return;
	}

	DrainCompletedResourceReleases();
	ownedRecord->bPendingRelease = true;
	m_pendingOwnedResourceReleases.push_back(std::move(ownedRecord));
}

void D3D12ResourceService::DrainCompletedResourceReleases() noexcept
{
	if (m_pendingOwnedResourceReleases.empty() && m_pendingOwnedMemoryBlockReleases.empty())
	{
		return;
	}

	std::array<std::uint64_t, RhiQueueTypeCount> completedValues{};
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		completedValues[queueIndex] = m_rhi.GetCompletedSubmissionValue(static_cast<ERhiQueueType>(queueIndex));
	}

	auto eraseBegin = std::remove_if(
	    m_pendingOwnedResourceReleases.begin(),
	    m_pendingOwnedResourceReleases.end(),
	    [&completedValues](const std::unique_ptr<D3D12GpuAllocationRecord>& record)
	    {
		    if (record == nullptr)
		    {
			    return true;
		    }

		    const bool bNoRecordingReferences = record->RecordingReferenceCount.load(std::memory_order_relaxed) == 0;
		    return bNoRecordingReferences && record->LastUse.IsComplete(completedValues);
	    });

	m_pendingOwnedResourceReleases.erase(eraseBegin, m_pendingOwnedResourceReleases.end());

	auto heapEraseBegin = std::remove_if(
	    m_pendingOwnedMemoryBlockReleases.begin(),
	    m_pendingOwnedMemoryBlockReleases.end(),
	    [&completedValues](const std::unique_ptr<D3D12GpuHeapRecord>& record)
	    {
		    if (record == nullptr)
		    {
			    return true;
		    }

		    const bool bUnreferenced = record->AliasingResourceCount == 0 && record->RecordingReferenceCount.load(std::memory_order_relaxed) == 0;
		    return bUnreferenced && record->LastUse.IsComplete(completedValues);
	    });

	m_pendingOwnedMemoryBlockReleases.erase(heapEraseBegin, m_pendingOwnedMemoryBlockReleases.end());
}

void D3D12ResourceService::FlushDeferredResourceReleases() noexcept
{
	DrainCompletedResourceReleases();
}

RhiResourceHandle D3D12ResourceService::GetResourceHandle(RhiOwnedResourceHandle resource) const noexcept
{
	return RhiResourceHandle{GetD3D12Resource(resource)};
}

RhiGpuVirtualAddress D3D12ResourceService::GetResourceGpuVirtualAddress(RhiOwnedResourceHandle resource) const noexcept
{
	ID3D12Resource* const nativeResource = GetD3D12Resource(resource);
	return nativeResource != nullptr ? nativeResource->GetGPUVirtualAddress() : 0;
}

RhiResourceAllocationInfo D3D12ResourceService::GetTextureAllocationInfo(const RhiTextureResourceDesc& desc) const noexcept
{
	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildTextureResourceDesc(desc);
	const D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = m_rhi.GetDevice()->GetResourceAllocationInfo(0, 1, &resourceDesc);
	return RhiResourceAllocationInfo{.SizeInBytes = allocationInfo.SizeInBytes, .Alignment = allocationInfo.Alignment};
}

RhiResourceAllocationInfo D3D12ResourceService::GetBufferAllocationInfo(const RhiBufferResourceDesc& desc) const noexcept
{
	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildBufferResourceDesc(desc);
	const D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = m_rhi.GetDevice()->GetResourceAllocationInfo(0, 1, &resourceDesc);
	return RhiResourceAllocationInfo{.SizeInBytes = allocationInfo.SizeInBytes, .Alignment = allocationInfo.Alignment};
}

RhiOwnedMemoryBlockHandle D3D12ResourceService::CreateTransientMemoryBlock(RhiTransientAllocationPool pool, std::uint64_t sizeInBytes, std::uint64_t alignment, std::wstring_view debugName)
{
	if (sizeInBytes == 0)
	{
		return {};
	}

	const std::wstring_view resourceDebugName = debugName.empty() ? L"TransientMemoryBlock" : debugName;
	std::unique_ptr<D3D12GpuHeapRecord> ownedMemoryBlock = m_memoryAllocator.CreateTransientHeap(pool, sizeInBytes, alignment, resourceDebugName);
	return MakeD3D12OwnedMemoryBlockHandle(std::move(ownedMemoryBlock));
}

void D3D12ResourceService::ReleaseTransientMemoryBlock(RhiOwnedMemoryBlockHandle memoryBlock) noexcept
{
	std::unique_ptr<D3D12GpuHeapRecord> ownedMemoryBlock = TakeD3D12OwnedMemoryBlockHandle(memoryBlock);
	if (ownedMemoryBlock == nullptr)
	{
		return;
	}

	DrainCompletedResourceReleases();
	m_pendingOwnedMemoryBlockReleases.push_back(std::move(ownedMemoryBlock));
}

RhiOwnedResourceHandle D3D12ResourceService::CreateAliasingTextureResource(
    RhiOwnedMemoryBlockHandle memoryBlock,
    std::uint64_t memoryBlockOffset,
    const RhiTransientTextureAllocationDesc& desc,
    std::wstring_view debugName)
{
	D3D12GpuHeapRecord* const ownedMemoryBlock = GetD3D12GpuHeapRecord(memoryBlock);
	const bool bUsableTexture = RhiContract::IsTextureResourceDescUsable(m_capabilities, desc.ResourceDesc);
	if (ownedMemoryBlock == nullptr || !bUsableTexture)
	{
		return {};
	}

	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildTextureResourceDesc(desc.ResourceDesc);
	const D3D12_RESOURCE_STATES resourceState = D3D12TypeConversions::ToResourceStates(desc.InitialState);
	const D3D12_CLEAR_VALUE clearValue = D3D12TypeConversions::BuildClearValue(desc.ClearValue);
	const D3D12_CLEAR_VALUE* clearValuePtr = desc.ClearValue.ValueType == RhiOptimizedClearValue::Type::None ? nullptr : &clearValue;
	const std::wstring_view resourceDebugName = debugName.empty() ? L"AliasingTexture" : debugName;

	std::unique_ptr<D3D12GpuAllocationRecord> ownedResource = m_memoryAllocator
	                                                              .CreateAliasingTexture(*ownedMemoryBlock, memoryBlockOffset, resourceDesc, resourceState, clearValuePtr, resourceDebugName);

	return MakeD3D12OwnedResourceHandle(std::move(ownedResource));
}

RhiOwnedResourceHandle D3D12ResourceService::CreateAliasingBufferResource(
    RhiOwnedMemoryBlockHandle memoryBlock,
    std::uint64_t memoryBlockOffset,
    const RhiTransientBufferAllocationDesc& desc,
    std::wstring_view debugName)
{
	D3D12GpuHeapRecord* const ownedMemoryBlock = GetD3D12GpuHeapRecord(memoryBlock);
	if (ownedMemoryBlock == nullptr)
	{
		return {};
	}

	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildBufferResourceDesc(desc.ResourceDesc);
	const D3D12_RESOURCE_STATES resourceState = D3D12TypeConversions::ToResourceStates(desc.InitialState);
	const std::wstring_view resourceDebugName = debugName.empty() ? L"AliasingBuffer" : debugName;

	std::unique_ptr<D3D12GpuAllocationRecord> ownedResource = m_memoryAllocator.CreateAliasingBuffer(*ownedMemoryBlock, memoryBlockOffset, resourceDesc, resourceState, resourceDebugName);

	return MakeD3D12OwnedResourceHandle(std::move(ownedResource));
}

bool D3D12ResourceService::SupportsUnorderedAccess(RhiResourceHandle resource) const noexcept
{
	ID3D12Resource* const nativeResource = static_cast<ID3D12Resource*>(resource.Value);
	return nativeResource != nullptr && (nativeResource->GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0;
}

D3D12RecordingResourceUseToken D3D12ResourceService::BeginResourceTracking(RhiResourceHandle resource, bool bCoordinatorRecording) noexcept
{
	if (!resource)
	{
		return {};
	}

	D3D12RecordingResourceUseToken use = m_memoryAllocator.RetainRecordingResource(resource);
	if (!use && bCoordinatorRecording)
	{
		use = m_memoryAllocator.RetainCoordinatorRecordingResource(resource);
	}
	return use;
}

void D3D12ResourceService::EndResourceTracking(D3D12RecordingResourceUseToken use, RhiSubmissionToken submissionToken) noexcept
{
	m_memoryAllocator.ReleaseRecordingResource(use, submissionToken);
}

void D3D12ResourceService::CollectCrashDiagnosticsOnce() noexcept
{
	if (m_bCrashDiagnosticsCollected || !m_rhi.SupportsCrashDiagnostics())
	{
		return;
	}

	m_bCrashDiagnosticsCollected = true;
	m_rhi.CollectCrashDiagnostics();

	RhiDiagnosticMessage message{};
	while (m_rhi.TryPopDebugMessage(message))
	{
		SPDLOG_LOGGER_WARN(LogD3D12Diagnostics.GetLogger(), "{}", message.Text);
	}
}
