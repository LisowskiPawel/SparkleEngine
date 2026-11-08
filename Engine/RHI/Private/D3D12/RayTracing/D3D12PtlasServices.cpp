#include "PCH.h"

#include "D3D12/RayTracing/D3D12PtlasServices.h"
#include "D3D12/RayTracing/D3D12PtlasOperationPacking.h"

#include "D3D12/D3D12TypeConversions.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Memory/D3D12GpuAllocation.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"
#include "D3D12/RayTracing/D3D12NvapiRayTracingProvider.h"
#include "Memory/RhiMemoryTypes.h"
#include "RayTracing/RhiPtlasOperationLayout.h"
#include "Resources/RhiResourceDesc.h"
#include "Validation/RhiContract.h"

#include <cstring>
#include <memory>
#include <span>
#include <vector>

D3D12PtlasServices::D3D12PtlasServices(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator, D3D12NvapiRayTracingProvider& nvapiProvider) noexcept :
    m_rhi(&rhi),
    m_memoryAllocator(&memoryAllocator),
    m_nvapiProvider(&nvapiProvider)
{
}

RhiPtlasBuildSizes D3D12PtlasServices::GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || m_nvapiProvider == nullptr || !capabilities.Groups.Ptlas.bSupported)
	{
		return {};
	}
	return m_nvapiProvider->GetPtlasBuildSizes(m_rhi->GetDevice().Get(), desc);
}

RhiOwnedResourceHandle D3D12PtlasServices::CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName)
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || m_memoryAllocator == nullptr || !capabilities.Groups.Ptlas.bSupported || sizes.AccelerationStructureSizeInBytes == 0)
	{
		return {};
	}

	const RhiBufferResourceDesc desc{
	    .SizeInBytes = RhiPtlasOperationLayout::AlignUp(sizes.AccelerationStructureSizeInBytes, capabilities.AccelerationStructureByteAlignment),
	    .bAllowUnorderedAccess = true};

	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildBufferResourceDesc(desc);

	std::unique_ptr<D3D12GpuAllocationRecord> ownedRecord = m_memoryAllocator->CreateBuffer(
	    resourceDesc,
	    D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
	    RhiMemoryCategory::RayTracing,
	    RhiMemoryResidencyClass::DeviceLocal,
	    debugName.empty() ? L"RayTracingPtlasStorage" : debugName);

	return ownedRecord != nullptr ? MakeD3D12OwnedResourceHandle(std::move(ownedRecord)) : RhiOwnedResourceHandle{};
}

static std::unique_ptr<D3D12GpuAllocationRecord> AllocateOperationStorage(D3D12GpuMemoryAllocator& memoryAllocator, std::uint64_t sizeInBytes, std::wstring_view debugName)
{
	const D3D12_RESOURCE_DESC resourceDesc = D3D12TypeConversions::BuildBufferResourceDesc(RhiBufferResourceDesc{.SizeInBytes = sizeInBytes});

	return memoryAllocator.CreateBuffer(
	    resourceDesc,
	    D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	    RhiMemoryCategory::RayTracing,
	    RhiMemoryResidencyClass::HostUpload,
	    debugName.empty() ? L"RayTracingPtlasOperations" : debugName);
}

static bool WriteOperationStorage(D3D12GpuAllocationRecord& record, std::span<const std::uint8_t> bytes)
{
	void* mappedData = nullptr;
	const D3D12_RANGE readRange{0, 0};
	if (FAILED(record.Resource->Map(0, &readRange, &mappedData)) || mappedData == nullptr)
	{
		return false;
	}
	record.bIsMapped = true;
	record.CpuMappedAddress = mappedData;
	std::memcpy(mappedData, bytes.data(), bytes.size());
	record.Resource->Unmap(0, nullptr);
	record.bIsMapped = false;
	record.CpuMappedAddress = nullptr;
	return true;
}

RhiOwnedResourceHandle D3D12PtlasServices::CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName)
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	const bool bOperationPackMemoryAllocator = m_rhi == nullptr || m_memoryAllocator == nullptr;
	const bool bInvalidOperationPackSupported = bOperationPackMemoryAllocator || (!capabilities.Groups.Ptlas.bSupported);
	const bool bInvalidPtlasOperationPack = bInvalidOperationPackSupported || (!RhiContract::IsPtlasOperationPackUsable(operationPack));

	if (bInvalidPtlasOperationPack)
	{
		return {};
	}

	const RhiPtlasNativeOperationLayout nativeLayout = D3D12PtlasOperationPacking::GetNativeLayout();

	const RhiPtlasOperationBufferLayout layout = RhiPtlasOperationLayout::Build(
	    operationPack.OperationCount,
	    operationPack.InstanceWriteCount,
	    operationPack.InstanceUpdateCount,
	    operationPack.PartitionTranslationCount,
	    nativeLayout);

	if (layout.TotalSizeInBytes == 0)
	{
		return {};
	}

	std::unique_ptr<D3D12GpuAllocationRecord> ownedRecord = AllocateOperationStorage(*m_memoryAllocator, layout.TotalSizeInBytes, debugName);

	if (ownedRecord == nullptr || ownedRecord->Resource == nullptr)
	{
		return {};
	}

	const std::vector<std::uint8_t> packed = D3D12PtlasOperationPacking::Pack(operationPack, layout, ownedRecord->Resource->GetGPUVirtualAddress());

	if (!WriteOperationStorage(*ownedRecord, packed))
	{
		return {};
	}
	return MakeD3D12OwnedResourceHandle(std::move(ownedRecord));
}

RhiPtlasOperationBufferLayout D3D12PtlasServices::GetPtlasOperationBufferLayout(const RhiPtlasDesc& desc) const noexcept
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || !capabilities.Groups.Ptlas.bSupported || desc.MaxOperations == 0)
	{
		return {};
	}

	return RhiPtlasOperationLayout::Build(
	    desc.MaxOperations,
	    desc.InstanceCapacity,
	    desc.bAllowInstanceUpdates ? desc.InstanceCapacity : 0u,
	    desc.bAllowPartitionTranslation ? desc.PartitionCount + 1u : 0u,
	    D3D12PtlasOperationPacking::GetNativeLayout());
}
