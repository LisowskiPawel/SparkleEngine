#include "Vulkan/VulkanPCH.h"

#include "Vulkan/RayTracing/VulkanPtlasServices.h"
#include "Vulkan/RayTracing/VulkanPtlasOperationPacking.h"

#include "Memory/RhiMemoryTypes.h"
#include "RayTracing/RhiPtlasOperationLayout.h"
#include "RayTracing/RhiRayTracingDesc.h"
#include "Resources/RhiResourceDesc.h"
#include "Validation/RhiContract.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Memory/VulkanGpuAllocation.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"
#include "Vulkan/VulkanTypeConversions.h"

#include <memory>
#include <vector>

VulkanPtlasServices::VulkanPtlasServices(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept :
    m_rhi(&rhi),
    m_memoryAllocator(&memoryAllocator)
{
}

RhiPtlasBuildSizes VulkanPtlasServices::GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	const bool bInvalidSizingInputsSupported = m_rhi == nullptr || !capabilities.Groups.Ptlas.bSupported;
	const bool bInvalidSizingInputsBuildSizes = bInvalidSizingInputsSupported || (m_rhi->GetPtlasBuildSizes() == nullptr);
	const bool bSizingInputsInstanceCapacity = bInvalidSizingInputsBuildSizes || (desc.InstanceCapacity == 0);
	const bool bInvalidPtlasSizingInputs = bSizingInputsInstanceCapacity || (desc.PartitionCount == 0);

	if (bInvalidPtlasSizingInputs)
	{
		return {};
	}

	VkPartitionedAccelerationStructureFlagsNV ptlasFlags{};
	VkPartitionedAccelerationStructureInstancesInputNV input{};
	VulkanTypeConversions::ConfigurePtlasInput(desc, input, ptlasFlags);
	VkAccelerationStructureBuildSizesInfoKHR nativeInfo{.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR, .pNext = nullptr};
	m_rhi->GetPtlasBuildSizes()(m_rhi->GetDevice(), &input, &nativeInfo);

	return RhiPtlasBuildSizes{
	    .AccelerationStructureSizeInBytes = nativeInfo.accelerationStructureSize,
	    .BuildScratchSizeInBytes = nativeInfo.buildScratchSize,
	    .UpdateScratchSizeInBytes = nativeInfo.updateScratchSize,
	    .OperationInfoSizeInBytes = sizeof(VkBuildPartitionedAccelerationStructureIndirectCommandNV) * static_cast<std::uint64_t>(desc.MaxOperations),
	    .OperationCountSizeInBytes = sizeof(std::uint32_t),
	    .InstanceWriteInfoSizeInBytes = sizeof(VkPartitionedAccelerationStructureWriteInstanceDataNV) * static_cast<std::uint64_t>(desc.InstanceCapacity),
	    .InstanceUpdateInfoSizeInBytes = desc.bAllowInstanceUpdates ? sizeof(VkPartitionedAccelerationStructureUpdateInstanceDataNV) * static_cast<std::uint64_t>(desc.InstanceCapacity) : 0u,
	    .PartitionWriteInfoSizeInBytes = desc.bAllowPartitionTranslation
	        ? sizeof(VkPartitionedAccelerationStructureWritePartitionTranslationDataNV) * static_cast<std::uint64_t>(desc.PartitionCount + 1u)
	        : 0u};
}

RhiOwnedResourceHandle VulkanPtlasServices::CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName)
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || m_memoryAllocator == nullptr || !capabilities.Groups.Ptlas.bSupported || sizes.AccelerationStructureSizeInBytes == 0)
	{
		return {};
	}

	const RhiBufferResourceDesc desc{
	    .SizeInBytes = RhiPtlasOperationLayout::AlignUp(sizes.AccelerationStructureSizeInBytes, capabilities.AccelerationStructureByteAlignment),
	    .bAllowUnorderedAccess = true};

	const VkBufferCreateInfo bufferCreateInfo = VulkanTypeConversions::BuildBufferCreateInfo(
	    desc,
	    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	std::unique_ptr<VulkanGpuAllocationRecord> record = m_memoryAllocator->CreateBuffer(
	    bufferCreateInfo,
	    RhiMemoryCategory::RayTracing,
	    RhiMemoryResidencyClass::DeviceLocal,
	    debugName.empty() ? L"RayTracingPtlasStorage" : debugName);

	if (record == nullptr || record->DeviceAddress == 0)
	{
		return {};
	}

	record->AccelerationStructureType = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	record->bIsPtlas = true;
	SetVulkanAllocationRecordDebugName(*record, debugName.empty() ? L"RayTracingPtlasStorage" : debugName);
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

static std::unique_ptr<VulkanGpuAllocationRecord> AllocateOperationStorage(VulkanGpuMemoryAllocator& memoryAllocator, std::uint64_t sizeInBytes, std::wstring_view debugName)
{
	const RhiBufferResourceDesc desc{.SizeInBytes = sizeInBytes};

	const VkBufferCreateInfo bufferCreateInfo = VulkanTypeConversions::BuildBufferCreateInfo(
	    desc,
	    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	return memoryAllocator.CreateBuffer(bufferCreateInfo, RhiMemoryCategory::RayTracing, RhiMemoryResidencyClass::HostUpload, debugName.empty() ? L"RayTracingPtlasOperations" : debugName);
}

RhiOwnedResourceHandle VulkanPtlasServices::CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName)
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	const bool bOperationPackMemoryAllocator = m_rhi == nullptr || m_memoryAllocator == nullptr;
	const bool bInvalidOperationPackSupported = bOperationPackMemoryAllocator || (!capabilities.Groups.Ptlas.bSupported);
	const bool bInvalidPtlasOperationPack = bInvalidOperationPackSupported || (!RhiContract::IsPtlasOperationPackUsable(operationPack));

	if (bInvalidPtlasOperationPack)
	{
		return {};
	}

	const RhiPtlasNativeOperationLayout nativeLayout = VulkanPtlasOperationPacking::GetNativeLayout();

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

	std::unique_ptr<VulkanGpuAllocationRecord> record = AllocateOperationStorage(*m_memoryAllocator, layout.TotalSizeInBytes, debugName);

	if (record == nullptr || record->Buffer == VK_NULL_HANDLE || record->DeviceAddress == 0)
	{
		return {};
	}

	const std::vector<std::uint8_t> packed = VulkanPtlasOperationPacking::Pack(operationPack, layout, record->DeviceAddress, *m_memoryAllocator);

	if (!m_memoryAllocator->WriteAllocation(*record, packed.data(), packed.size()))
	{
		return {};
	}
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

RhiPtlasOperationBufferLayout VulkanPtlasServices::GetPtlasOperationBufferLayout(const RhiPtlasDesc& desc) const noexcept
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
	    VulkanPtlasOperationPacking::GetNativeLayout());
}
