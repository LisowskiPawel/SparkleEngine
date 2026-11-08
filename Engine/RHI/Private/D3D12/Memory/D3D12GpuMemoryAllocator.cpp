#include "PCH.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "D3D12/Diagnostics/GpuCrash/D3D12GpuCrash.h"
#endif

#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"

#include "Core/Public/Formatting/HexFormat.h"
#include "D3D12/Memory/D3D12RecordingResourceTable.h"
#include "D3D12/Memory/D3D12MemoryUsage.h"
#include "Core/Public/Strings/StringUtils.h"

#include <D3D12MemAlloc.h>

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>
#include <Windows.h>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_d3d12MemoryLogger, "RHI.D3D12.Memory");

struct D3D12GpuMemoryAllocator::Impl
{
	D3D12MA::Allocator* allocator = nullptr;
	mutable std::mutex recordsMutex;
	std::vector<D3D12GpuAllocationRecord*> liveRecords;
	std::vector<D3D12GpuHeapRecord*> liveHeapRecords;

	~Impl() noexcept
	{
		if (allocator != nullptr)
		{
			allocator->Release();
			allocator = nullptr;
		}
	}
};

D3D12GpuMemoryAllocator::D3D12GpuMemoryAllocator(IDXGIAdapter* adapter, ID3D12Device* device) noexcept :
    m_impl(std::make_unique<Impl>()),
    m_recordingResources(std::make_unique<D3D12RecordingResourceTable>())
{
	if (adapter == nullptr || device == nullptr)
	{
		Diagnostics::Fatal(g_d3d12MemoryLogger, __FILE__, __LINE__, "D3D12GpuMemoryAllocator requires a valid adapter and device");
	}

	D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
	allocatorDesc.pAdapter = adapter;
	allocatorDesc.pDevice = device;

	CHECK(D3D12MA::CreateAllocator(&allocatorDesc, &m_impl->allocator));
}

D3D12GpuMemoryAllocator::~D3D12GpuMemoryAllocator() noexcept = default;

bool D3D12GpuMemoryAllocator::IsInitialized() const noexcept
{
	return m_impl != nullptr && m_impl->allocator != nullptr;
}

bool D3D12GpuMemoryAllocator::SupportsBudgetQueries() const noexcept
{
	return IsInitialized();
}

RhiMemoryUsageSnapshot D3D12GpuMemoryAllocator::CreateMemoryUsageSnapshot() const
{
	m_owner.AssertAccess();
	RhiMemoryUsageSnapshot snapshot;
	if (m_impl == nullptr || m_impl->allocator == nullptr)
	{
		return snapshot;
	}

	D3D12MA::Budget localBudget = {};
	D3D12MA::Budget nonLocalBudget = {};
	D3D12MemoryUsage::QueryBudget(*m_impl->allocator, localBudget, nonLocalBudget, snapshot);

	std::vector<D3D12MemoryCategoryAggregation> aggregations;
	{
		std::scoped_lock lock(m_impl->recordsMutex);
		aggregations.reserve(m_impl->liveRecords.size());
		D3D12MemoryUsage::AccumulateAllocations(m_impl->liveRecords, localBudget, nonLocalBudget, aggregations, snapshot);
		D3D12MemoryUsage::AccumulateHeaps(m_impl->liveHeapRecords, localBudget, nonLocalBudget, aggregations, snapshot);
	}
	D3D12MemoryUsage::AppendCategories(aggregations, snapshot);
	return snapshot;
}

std::unique_ptr<D3D12GpuAllocationRecord> D3D12GpuMemoryAllocator::CreateTexture(
    const D3D12_RESOURCE_DESC& resourceDesc,
    D3D12_RESOURCE_STATES initialState,
    const D3D12_CLEAR_VALUE* optimizedClearValue,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName) noexcept
{
	return CreateResource(resourceDesc, initialState, optimizedClearValue, category, residencyClass, debugName);
}

std::unique_ptr<D3D12GpuAllocationRecord> D3D12GpuMemoryAllocator::CreateBuffer(
    const D3D12_RESOURCE_DESC& resourceDesc,
    D3D12_RESOURCE_STATES initialState,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName) noexcept
{
	return CreateResource(resourceDesc, initialState, nullptr, category, residencyClass, debugName);
}

std::unique_ptr<D3D12GpuHeapRecord> D3D12GpuMemoryAllocator::CreateTransientHeap(
    RhiTransientAllocationPool pool,
    std::uint64_t sizeInBytes,
    std::uint64_t alignment,
    std::wstring_view debugName) noexcept
{
	if (m_impl == nullptr || m_impl->allocator == nullptr || sizeInBytes == 0)
	{
		return {};
	}

	D3D12MA::ALLOCATION_DESC allocationDesc = {};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	allocationDesc.ExtraHeapFlags = ToTransientHeapFlags(pool);

	D3D12_RESOURCE_ALLOCATION_INFO allocationInfo{};
	allocationInfo.SizeInBytes = sizeInBytes;
	allocationInfo.Alignment = alignment;

	D3D12MA::Allocation* allocation = nullptr;
	const HRESULT hr = m_impl->allocator->AllocateMemory(&allocationDesc, &allocationInfo, &allocation);
	if (FAILED(hr) || allocation == nullptr || allocation->GetHeap() == nullptr)
	{
		if (allocation != nullptr)
		{
			allocation->Release();
		}
		return {};
	}

	auto record = std::make_unique<D3D12GpuHeapRecord>();
	record->NativeHeap = allocation->GetHeap();
	record->Allocation = allocation;
	record->Category = RhiMemoryCategory::TransientResource;
	record->ResidencyClass = RhiMemoryResidencyClass::Transient;
	record->Owner = this;
	record->DebugName = std::wstring(debugName);
	if (!record->DebugName.empty())
	{
		record->NativeHeap->SetName(record->DebugName.c_str());
		record->Allocation->SetName(record->DebugName.c_str());
	}
	RegisterHeapRecord(*record);

	return record;
}

std::unique_ptr<D3D12GpuAllocationRecord> D3D12GpuMemoryAllocator::CreateAliasingTexture(
    D3D12GpuHeapRecord& heap,
    std::uint64_t heapOffset,
    const D3D12_RESOURCE_DESC& resourceDesc,
    D3D12_RESOURCE_STATES initialState,
    const D3D12_CLEAR_VALUE* optimizedClearValue,
    std::wstring_view debugName) noexcept
{
	return CreateAliasingResource(heap, heapOffset, resourceDesc, initialState, optimizedClearValue, debugName);
}

std::unique_ptr<D3D12GpuAllocationRecord> D3D12GpuMemoryAllocator::CreateAliasingBuffer(
    D3D12GpuHeapRecord& heap,
    std::uint64_t heapOffset,
    const D3D12_RESOURCE_DESC& resourceDesc,
    D3D12_RESOURCE_STATES initialState,
    std::wstring_view debugName) noexcept
{
	return CreateAliasingResource(heap, heapOffset, resourceDesc, initialState, nullptr, debugName);
}

std::unique_ptr<D3D12GpuAllocationRecord> D3D12GpuMemoryAllocator::CreateResource(
    const D3D12_RESOURCE_DESC& resourceDesc,
    D3D12_RESOURCE_STATES initialState,
    const D3D12_CLEAR_VALUE* optimizedClearValue,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName) noexcept
{
	if (m_impl == nullptr || m_impl->allocator == nullptr)
	{
		return {};
	}

	D3D12MA::ALLOCATION_DESC allocationDesc = {};
	allocationDesc.HeapType = ToHeapType(residencyClass);

	D3D12MA::Allocation* allocation = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	const HRESULT hr = m_impl->allocator->CreateResource(&allocationDesc, &resourceDesc, initialState, optimizedClearValue, &allocation, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));
	if (FAILED(hr) || allocation == nullptr || resource == nullptr)
	{
		std::size_t liveAllocationCount = 0;
		std::size_t liveHeapCount = 0;
		{
			std::scoped_lock lock(m_impl->recordsMutex);
			liveAllocationCount = m_impl->liveRecords.size();
			liveHeapCount = m_impl->liveHeapRecords.size();
		}

		SPDLOG_LOGGER_WARN(
		    g_d3d12MemoryLogger,
		    "D3D12 allocation failed: hr={} bytes={} category={} residency={} liveAllocations={} liveHeaps={} name='{}'",
		    Formatting::FormatPrefixedHexUInt32(static_cast<std::uint32_t>(hr)),
		    static_cast<std::uint64_t>(resourceDesc.Width),
		    RhiMemoryCategoryName(category),
		    RhiMemoryResidencyClassName(residencyClass),
		    liveAllocationCount,
		    liveHeapCount,
		    Strings::ToNarrow(debugName));

		if (allocation != nullptr)
		{
			allocation->Release();
		}
		return {};
	}

	auto record = std::make_unique<D3D12GpuAllocationRecord>();
	record->Resource = std::move(resource);
	record->Allocation = allocation;
	record->Category = category;
	record->ResidencyClass = residencyClass;
	record->Owner = this;
	record->DebugName = std::wstring(debugName);
	if (!record->DebugName.empty())
	{
		record->Resource->SetName(record->DebugName.c_str());
		record->Allocation->SetName(record->DebugName.c_str());
	}
	RegisterAllocationRecord(*record);

	return record;
}

std::unique_ptr<D3D12GpuAllocationRecord> D3D12GpuMemoryAllocator::CreateAliasingResource(
    D3D12GpuHeapRecord& heap,
    std::uint64_t heapOffset,
    const D3D12_RESOURCE_DESC& resourceDesc,
    D3D12_RESOURCE_STATES initialState,
    const D3D12_CLEAR_VALUE* optimizedClearValue,
    std::wstring_view debugName) noexcept
{
	if (m_impl == nullptr || m_impl->allocator == nullptr || heap.Allocation == nullptr)
	{
		return {};
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	const HRESULT hr = m_impl->allocator->CreateAliasingResource(heap.Allocation, heapOffset, &resourceDesc, initialState, optimizedClearValue, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));
	if (FAILED(hr) || resource == nullptr)
	{
		return {};
	}

	auto record = std::make_unique<D3D12GpuAllocationRecord>();
	record->Resource = std::move(resource);
	record->Allocation = heap.Allocation;
	record->Allocation->AddRef();
	record->ParentHeap = &heap;
	record->Owner = this;
	record->Category = RhiMemoryCategory::TransientResource;
	record->ResidencyClass = RhiMemoryResidencyClass::Transient;
	record->DebugName = std::wstring(debugName);
	++heap.AliasingResourceCount;
	if (!record->DebugName.empty())
	{
		record->Resource->SetName(record->DebugName.c_str());
	}
	RegisterAllocationRecord(*record);

	return record;
}

D3D12_HEAP_TYPE D3D12GpuMemoryAllocator::ToHeapType(RhiMemoryResidencyClass residencyClass) noexcept
{
	switch (residencyClass)
	{
		case RhiMemoryResidencyClass::HostUpload:
			return D3D12_HEAP_TYPE_UPLOAD;
		case RhiMemoryResidencyClass::HostReadback:
			return D3D12_HEAP_TYPE_READBACK;
		case RhiMemoryResidencyClass::DeviceLocal:
		case RhiMemoryResidencyClass::Transient:
		default:
			return D3D12_HEAP_TYPE_DEFAULT;
	}
}

D3D12_HEAP_FLAGS D3D12GpuMemoryAllocator::ToTransientHeapFlags(RhiTransientAllocationPool pool) noexcept
{
	switch (pool)
	{
		case RhiTransientAllocationPool::Buffer:
			return D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
		case RhiTransientAllocationPool::Texture:
			return D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
		case RhiTransientAllocationPool::RenderTargetTexture:
		case RhiTransientAllocationPool::DepthStencilTexture:
		default:
			return D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
	}
}

D3D12GpuAllocationRecord* D3D12GpuMemoryAllocator::FindAllocationRecord(ID3D12Resource* resource) const noexcept
{
	m_owner.AssertAccess();
	if (m_impl == nullptr || resource == nullptr)
	{
		return nullptr;
	}

	std::scoped_lock lock(m_impl->recordsMutex);

	const auto record = std::find_if(
	    m_impl->liveRecords.begin(),
	    m_impl->liveRecords.end(),
	    [resource](const D3D12GpuAllocationRecord* candidate) noexcept { return candidate != nullptr && !candidate->bPendingRelease && candidate->Resource.Get() == resource; });

	return record != m_impl->liveRecords.end() ? *record : nullptr;
}

void D3D12GpuMemoryAllocator::PublishRecordingReadView() noexcept
{
	m_owner.AssertAccess();
	if (m_impl != nullptr)
	{
		m_recordingResources->Publish(m_impl->liveRecords);
	}
}

void D3D12GpuMemoryAllocator::ClearRecordingReadView() noexcept
{
	m_owner.AssertAccess();
	m_recordingResources->Publish(std::span<D3D12GpuAllocationRecord* const>{});
}

D3D12RecordingResourceUseToken D3D12GpuMemoryAllocator::RetainRecordingResource(RhiResourceHandle resource) const noexcept
{
	return m_recordingResources->Retain(resource);
}

D3D12RecordingResourceUseToken D3D12GpuMemoryAllocator::RetainCoordinatorRecordingResource(RhiResourceHandle resource) const noexcept
{
	D3D12GpuAllocationRecord* const record = FindAllocationRecord(static_cast<ID3D12Resource*>(resource.Value));
	return record != nullptr ? m_recordingResources->Retain(*record) : D3D12RecordingResourceUseToken{};
}

void D3D12GpuMemoryAllocator::ReleaseRecordingResource(D3D12RecordingResourceUseToken use, RhiSubmissionToken submissionToken) const noexcept
{
	m_recordingResources->Release(use, submissionToken);
}

void D3D12GpuMemoryAllocator::RegisterAllocationRecord(D3D12GpuAllocationRecord& record) noexcept
{
	m_owner.AssertAccess();
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	D3D12GpuCrash::Resource(record.Resource.Get());
#endif
	if (m_impl == nullptr)
	{
		return;
	}

	std::scoped_lock lock(m_impl->recordsMutex);
	if (std::find(m_impl->liveRecords.begin(), m_impl->liveRecords.end(), &record) == m_impl->liveRecords.end())
	{
		m_impl->liveRecords.push_back(&record);
	}
}

void D3D12GpuMemoryAllocator::UnregisterAllocationRecord(D3D12GpuAllocationRecord& record) noexcept
{
	m_owner.AssertAccess();
	if (m_impl == nullptr)
	{
		return;
	}

	std::scoped_lock lock(m_impl->recordsMutex);
	auto eraseBegin = std::remove(m_impl->liveRecords.begin(), m_impl->liveRecords.end(), &record);
	m_impl->liveRecords.erase(eraseBegin, m_impl->liveRecords.end());
}

void D3D12GpuMemoryAllocator::RegisterHeapRecord(D3D12GpuHeapRecord& record) noexcept
{
	m_owner.AssertAccess();
	if (m_impl == nullptr)
	{
		return;
	}

	std::scoped_lock lock(m_impl->recordsMutex);
	if (std::find(m_impl->liveHeapRecords.begin(), m_impl->liveHeapRecords.end(), &record) == m_impl->liveHeapRecords.end())
	{
		m_impl->liveHeapRecords.push_back(&record);
	}
}

void D3D12GpuMemoryAllocator::UnregisterHeapRecord(D3D12GpuHeapRecord& record) noexcept
{
	m_owner.AssertAccess();
	if (m_impl == nullptr)
	{
		return;
	}

	std::scoped_lock lock(m_impl->recordsMutex);
	auto eraseBegin = std::remove(m_impl->liveHeapRecords.begin(), m_impl->liveHeapRecords.end(), &record);
	m_impl->liveHeapRecords.erase(eraseBegin, m_impl->liveHeapRecords.end());
}
