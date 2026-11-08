#include "Vulkan/VulkanPCH.h"

#include "Vulkan/RayTracing/VulkanPtlasOperationPacking.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"

#include <cstring>

static VkPartitionedAccelerationStructureInstanceFlagsNV ToVkPtlasInstanceFlags(RhiPtlasInstanceFlags flags) noexcept
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

static VkPartitionedAccelerationStructureOpTypeNV ToVkPtlasOperationType(ERhiPtlasOperationType type) noexcept
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

RhiPtlasNativeOperationLayout VulkanPtlasOperationPacking::GetNativeLayout() noexcept
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

static RhiGpuVirtualAddress ResolveInstanceAddress(const VulkanGpuMemoryAllocator& memoryAllocator, RhiGpuVirtualAddress accelerationStructure) noexcept
{
	if (accelerationStructure == 0)
	{
		return accelerationStructure;
	}

	const VulkanGpuAllocationRecord* const record = memoryAllocator.FindAllocationRecordByDeviceAddress(accelerationStructure);
	if (record == nullptr || record->AccelerationStructure == VK_NULL_HANDLE || record->BufferDeviceAddress == 0)
	{
		return accelerationStructure;
	}

	return record->BufferDeviceAddress;
}

static void PackOperations(
    const RhiPtlasOperationPackDesc& operationPack,
    VkBuildPartitionedAccelerationStructureIndirectCommandNV* nativeOperations,
    const RhiPtlasOperationBufferLayout& layout,
    RhiGpuVirtualAddress baseAddress)
{
	const RhiPtlasNativeOperationLayout nativeLayout = VulkanPtlasOperationPacking::GetNativeLayout();
	const RhiGpuVirtualAddress instanceWriteAddress = baseAddress + layout.InstanceWriteRecordsOffsetInBytes;
	const RhiGpuVirtualAddress instanceUpdateAddress = baseAddress + layout.InstanceUpdateRecordsOffsetInBytes;
	const RhiGpuVirtualAddress partitionTranslationAddress = baseAddress + layout.PartitionTranslationRecordsOffsetInBytes;
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
}

static void PackInstanceWrites(
    const RhiPtlasOperationPackDesc& operationPack,
    VkPartitionedAccelerationStructureWriteInstanceDataNV* nativeInstanceWrites,
    const VulkanGpuMemoryAllocator& memoryAllocator)
{
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
		target.accelerationStructure = ResolveInstanceAddress(memoryAllocator, source.AccelerationStructure);
	}
}

static void PackInstanceUpdates(
    const RhiPtlasOperationPackDesc& operationPack,
    VkPartitionedAccelerationStructureUpdateInstanceDataNV* nativeInstanceUpdates,
    const VulkanGpuMemoryAllocator& memoryAllocator)
{
	for (std::uint32_t instanceIndex = 0; instanceIndex < operationPack.InstanceUpdateCount; ++instanceIndex)
	{
		const RhiPtlasInstanceUpdateDesc& source = operationPack.InstanceUpdates[instanceIndex];

		nativeInstanceUpdates[instanceIndex] = VkPartitionedAccelerationStructureUpdateInstanceDataNV{
		    .instanceIndex = source.InstanceIndex,
		    .instanceContributionToHitGroupIndex = source.InstanceContributionToHitGroupIndex,
		    .accelerationStructure = ResolveInstanceAddress(memoryAllocator, source.AccelerationStructure)};
	}
}

static void PackPartitionTranslations(const RhiPtlasOperationPackDesc& operationPack, VkPartitionedAccelerationStructureWritePartitionTranslationDataNV* nativePartitionTranslations)
{
	for (std::uint32_t partitionIndex = 0; partitionIndex < operationPack.PartitionTranslationCount; ++partitionIndex)
	{
		const RhiPtlasPartitionTranslationDesc& source = operationPack.PartitionTranslations[partitionIndex];
		VkPartitionedAccelerationStructureWritePartitionTranslationDataNV& target = nativePartitionTranslations[partitionIndex];
		target.partitionIndex = source.PartitionIndex;
		target.partitionTranslation[0] = source.Translation[0];
		target.partitionTranslation[1] = source.Translation[1];
		target.partitionTranslation[2] = source.Translation[2];
	}
}

std::vector<std::uint8_t> VulkanPtlasOperationPacking::Pack(
    const RhiPtlasOperationPackDesc& operationPack,
    const RhiPtlasOperationBufferLayout& layout,
    RhiGpuVirtualAddress baseAddress,
    const VulkanGpuMemoryAllocator& memoryAllocator)
{
	std::vector<std::uint8_t> packed(static_cast<std::size_t>(layout.TotalSizeInBytes), 0);
	std::memcpy(packed.data() + layout.OperationCountOffsetInBytes, &operationPack.OperationCount, sizeof(operationPack.OperationCount));

	auto* const nativeOperations = reinterpret_cast<VkBuildPartitionedAccelerationStructureIndirectCommandNV*>(packed.data() + static_cast<std::size_t>(layout.OperationHeadersOffsetInBytes));
	auto* const nativeInstanceWrites = reinterpret_cast<VkPartitionedAccelerationStructureWriteInstanceDataNV*>(packed.data() + static_cast<std::size_t>(layout.InstanceWriteRecordsOffsetInBytes));
	auto* const nativeInstanceUpdates = reinterpret_cast<VkPartitionedAccelerationStructureUpdateInstanceDataNV*>(packed.data() + static_cast<std::size_t>(layout.InstanceUpdateRecordsOffsetInBytes));

	auto* const nativePartitionTranslations = reinterpret_cast<VkPartitionedAccelerationStructureWritePartitionTranslationDataNV*>(
	    packed.data() + static_cast<std::size_t>(layout.PartitionTranslationRecordsOffsetInBytes));

	PackOperations(operationPack, nativeOperations, layout, baseAddress);
	PackInstanceWrites(operationPack, nativeInstanceWrites, memoryAllocator);
	PackInstanceUpdates(operationPack, nativeInstanceUpdates, memoryAllocator);
	PackPartitionTranslations(operationPack, nativePartitionTranslations);
	return packed;
}
