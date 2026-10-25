#include "Vulkan/VulkanPCH.h"

#include "Vulkan/RayTracing/VulkanPtlasServices.h"

#include "Memory/RhiMemoryTypes.h"
#include "RayTracing/RhiPtlasOperationLayout.h"
#include "RayTracing/RhiRayTracingDesc.h"
#include "Resources/RhiResourceDesc.h"
#include "Validation/RhiContract.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Memory/VulkanGpuAllocation.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"
#include "Vulkan/VulkanTypeConversions.h"

#include <cstring>
#include <memory>
#include <vector>

VkPartitionedAccelerationStructureInstanceFlagsNV VulkanPtlasServices::ToVkPtlasInstanceFlags(RhiPtlasInstanceFlags flags) noexcept
{
	VkPartitionedAccelerationStructureInstanceFlagsNV nativeFlags = 0;
	if (HasFlag(flags, RhiPtlasInstanceFlags::TriangleFacingCullDisable))
	{
		nativeFlags |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE_BIT_NV;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::TriangleFlipFacing))
	{
		nativeFlags |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FLIP_FACING_BIT_NV;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::ForceOpaque))
	{
		nativeFlags |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE_BIT_NV;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::ForceNoOpaque))
	{
		nativeFlags |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE_BIT_NV;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::EnableExplicitBoundingBox))
	{
		nativeFlags |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_ENABLE_EXPLICIT_BOUNDING_BOX_NV;
	}
	return nativeFlags;
}

VkPartitionedAccelerationStructureOpTypeNV VulkanPtlasServices::ToVkPtlasOperationType(ERhiPtlasOperationType type) noexcept
{
	switch (type)
	{
		case ERhiPtlasOperationType::UpdateInstance:
			return VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_UPDATE_INSTANCE_NV;
		case ERhiPtlasOperationType::WritePartitionTranslation:
			return VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_WRITE_PARTITION_TRANSLATION_NV;
		case ERhiPtlasOperationType::WriteInstance:
		default:
			return VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_WRITE_INSTANCE_NV;
	}
}

RhiPtlasNativeOperationLayout VulkanPtlasServices::GetNativeOperationLayout() noexcept
{
	return RhiPtlasNativeOperationLayout{
	    .OperationCountSizeInBytes = sizeof(std::uint32_t),
	    .OperationHeaderStrideInBytes = sizeof(VkBuildPartitionedAccelerationStructureIndirectCommandNV),
	    .OperationHeaderAlignmentInBytes = alignof(VkBuildPartitionedAccelerationStructureIndirectCommandNV),
	    .InstanceWriteStrideInBytes = sizeof(VkPartitionedAccelerationStructureWriteInstanceDataNV),
	    .InstanceWriteAlignmentInBytes = alignof(VkPartitionedAccelerationStructureWriteInstanceDataNV),
	    .InstanceUpdateStrideInBytes = sizeof(VkPartitionedAccelerationStructureUpdateInstanceDataNV),
	    .InstanceUpdateAlignmentInBytes = alignof(VkPartitionedAccelerationStructureUpdateInstanceDataNV),
	    .PartitionTranslationStrideInBytes = sizeof(VkPartitionedAccelerationStructureWritePartitionTranslationDataNV),
	    .PartitionTranslationAlignmentInBytes = alignof(VkPartitionedAccelerationStructureWritePartitionTranslationDataNV),
	    .BufferAlignmentInBytes = 16};
}

RhiGpuVirtualAddress VulkanPtlasServices::ResolvePtlasInstanceAccelerationStructureAddress(RhiGpuVirtualAddress accelerationStructure) const noexcept
{
	if (m_memoryAllocator == nullptr || accelerationStructure == 0)
	{
		return accelerationStructure;
	}

	const VulkanGpuAllocationRecord* const record = m_memoryAllocator->FindAllocationRecordByDeviceAddress(accelerationStructure);
	if (record == nullptr || record->AccelerationStructure == VK_NULL_HANDLE || record->BufferDeviceAddress == 0)
	{
		return accelerationStructure;
	}

	return record->BufferDeviceAddress;
}

VulkanPtlasServices::VulkanPtlasServices(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept :
    m_rhi(&rhi),
    m_memoryAllocator(&memoryAllocator)
{
}

RhiPtlasBuildSizes VulkanPtlasServices::GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || !capabilities.Groups.Ptlas.Supported || m_rhi->GetPtlasBuildSizes() == nullptr || desc.InstanceCapacity == 0 || desc.PartitionCount == 0)
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
	    .InstanceUpdateInfoSizeInBytes = desc.AllowInstanceUpdates ? sizeof(VkPartitionedAccelerationStructureUpdateInstanceDataNV) * static_cast<std::uint64_t>(desc.InstanceCapacity) : 0u,
	    .PartitionWriteInfoSizeInBytes = desc.AllowPartitionTranslation
	        ? sizeof(VkPartitionedAccelerationStructureWritePartitionTranslationDataNV) * static_cast<std::uint64_t>(desc.PartitionCount + 1u)
	        : 0u};
}

RhiOwnedResourceHandle VulkanPtlasServices::CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName)
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || m_memoryAllocator == nullptr || !capabilities.Groups.Ptlas.Supported || sizes.AccelerationStructureSizeInBytes == 0)
	{
		return {};
	}

	const RhiBufferResourceDesc desc{
	    .SizeInBytes = RhiPtlasOperationLayout::AlignUp(sizes.AccelerationStructureSizeInBytes, capabilities.AccelerationStructureByteAlignment),
	    .AllowUnorderedAccess = true};

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
	record->IsPtlas = true;
	SetVulkanAllocationRecordDebugName(*record, debugName.empty() ? L"RayTracingPtlasStorage" : debugName);
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

RhiOwnedResourceHandle VulkanPtlasServices::CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName)
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || m_memoryAllocator == nullptr || !capabilities.Groups.Ptlas.Supported || !RhiContract::IsPtlasOperationPackUsable(operationPack))
	{
		return {};
	}

	const RhiPtlasNativeOperationLayout nativeLayout = GetNativeOperationLayout();

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

	const RhiBufferResourceDesc desc{.SizeInBytes = layout.TotalSizeInBytes};

	const VkBufferCreateInfo bufferCreateInfo = VulkanTypeConversions::BuildBufferCreateInfo(
	    desc,
	    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	std::unique_ptr<VulkanGpuAllocationRecord> record = m_memoryAllocator->CreateBuffer(
	    bufferCreateInfo,
	    RhiMemoryCategory::RayTracing,
	    RhiMemoryResidencyClass::HostUpload,
	    debugName.empty() ? L"RayTracingPtlasOperations" : debugName);

	if (record == nullptr || record->Buffer == VK_NULL_HANDLE || record->DeviceAddress == 0)
	{
		return {};
	}

	std::vector<std::uint8_t> packed(static_cast<std::size_t>(layout.TotalSizeInBytes), 0);
	std::memcpy(packed.data() + layout.OperationCountOffsetInBytes, &operationPack.OperationCount, sizeof(operationPack.OperationCount));

	auto* const nativeOperations = reinterpret_cast<VkBuildPartitionedAccelerationStructureIndirectCommandNV*>(packed.data() + static_cast<std::size_t>(layout.OperationHeadersOffsetInBytes));
	auto* const nativeInstanceWrites = reinterpret_cast<VkPartitionedAccelerationStructureWriteInstanceDataNV*>(packed.data() + static_cast<std::size_t>(layout.InstanceWriteRecordsOffsetInBytes));
	auto* const nativeInstanceUpdates = reinterpret_cast<VkPartitionedAccelerationStructureUpdateInstanceDataNV*>(packed.data() + static_cast<std::size_t>(layout.InstanceUpdateRecordsOffsetInBytes));

	auto* const nativePartitionTranslations = reinterpret_cast<VkPartitionedAccelerationStructureWritePartitionTranslationDataNV*>(
	    packed.data() + static_cast<std::size_t>(layout.PartitionTranslationRecordsOffsetInBytes));

	const RhiGpuVirtualAddress instanceWriteAddress = record->DeviceAddress + layout.InstanceWriteRecordsOffsetInBytes;
	const RhiGpuVirtualAddress instanceUpdateAddress = record->DeviceAddress + layout.InstanceUpdateRecordsOffsetInBytes;
	const RhiGpuVirtualAddress partitionTranslationAddress = record->DeviceAddress + layout.PartitionTranslationRecordsOffsetInBytes;
	for (std::uint32_t operationIndex = 0; operationIndex < operationPack.OperationCount; ++operationIndex)
	{
		const RhiPtlasOperationHeader& operation = operationPack.Operations[operationIndex];

		nativeOperations[operationIndex] = VkBuildPartitionedAccelerationStructureIndirectCommandNV{
		    .opType = ToVkPtlasOperationType(operation.Type),
		    .argCount = operation.ArgumentCount,
		    .argData = VkStridedDeviceAddressNV{
		        .startAddress = RhiPtlasOperationLayout::ResolveArgumentAddress(operation, instanceWriteAddress, instanceUpdateAddress, partitionTranslationAddress),
		        .strideInBytes = RhiPtlasOperationLayout::ResolveArgumentStride(operation, nativeLayout)}};
	}

	for (std::uint32_t instanceIndex = 0; instanceIndex < operationPack.InstanceWriteCount; ++instanceIndex)
	{
		const RhiPtlasInstanceWriteDesc& source = operationPack.InstanceWrites[instanceIndex];
		VkPartitionedAccelerationStructureWriteInstanceDataNV& target = nativeInstanceWrites[instanceIndex];
		for (std::uint32_t transformIndex = 0; transformIndex < source.Transform.size(); ++transformIndex)
		{
			target.transform.matrix[transformIndex / 4][transformIndex % 4] = source.Transform[transformIndex];
		}
		std::memcpy(target.explicitAABB, source.ExplicitBoundingBox.data(), sizeof(target.explicitAABB));
		target.instanceID = source.InstanceID;
		target.instanceMask = source.InstanceMask;
		target.instanceContributionToHitGroupIndex = source.InstanceContributionToHitGroupIndex;
		target.instanceFlags = ToVkPtlasInstanceFlags(source.Flags);
		target.instanceIndex = source.InstanceIndex;
		target.partitionIndex = source.PartitionIndex;
		target.accelerationStructure = ResolvePtlasInstanceAccelerationStructureAddress(source.AccelerationStructure);
	}

	for (std::uint32_t instanceIndex = 0; instanceIndex < operationPack.InstanceUpdateCount; ++instanceIndex)
	{
		const RhiPtlasInstanceUpdateDesc& source = operationPack.InstanceUpdates[instanceIndex];

		nativeInstanceUpdates[instanceIndex] = VkPartitionedAccelerationStructureUpdateInstanceDataNV{
		    .instanceIndex = source.InstanceIndex,
		    .instanceContributionToHitGroupIndex = source.InstanceContributionToHitGroupIndex,
		    .accelerationStructure = ResolvePtlasInstanceAccelerationStructureAddress(source.AccelerationStructure)};
	}

	for (std::uint32_t partitionIndex = 0; partitionIndex < operationPack.PartitionTranslationCount; ++partitionIndex)
	{
		const RhiPtlasPartitionTranslationDesc& source = operationPack.PartitionTranslations[partitionIndex];
		VkPartitionedAccelerationStructureWritePartitionTranslationDataNV& target = nativePartitionTranslations[partitionIndex];
		target.partitionIndex = source.PartitionIndex;
		target.partitionTranslation[0] = source.Translation[0];
		target.partitionTranslation[1] = source.Translation[1];
		target.partitionTranslation[2] = source.Translation[2];
	}

	if (!m_memoryAllocator->WriteAllocation(*record, packed.data(), packed.size()))
	{
		return {};
	}
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

RhiPtlasOperationBufferLayout VulkanPtlasServices::GetPtlasOperationBufferLayout(const RhiPtlasDesc& desc) const noexcept
{
	const RhiRayTracingCapabilities capabilities = m_rhi != nullptr ? m_rhi->GetRayTracingCapabilities() : RhiRayTracingCapabilities{};
	if (m_rhi == nullptr || !capabilities.Groups.Ptlas.Supported || desc.MaxOperations == 0)
	{
		return {};
	}

	return RhiPtlasOperationLayout::Build(
	    desc.MaxOperations,
	    desc.InstanceCapacity,
	    desc.AllowInstanceUpdates ? desc.InstanceCapacity : 0u,
	    desc.AllowPartitionTranslation ? desc.PartitionCount + 1u : 0u,
	    GetNativeOperationLayout());
}
