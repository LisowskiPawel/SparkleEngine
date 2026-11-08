#include "PCH.h"

#include "D3D12/RayTracing/D3D12PtlasOperationPacking.h"

#include <cstring>

#if SPARKLE_RHI_WITH_D3D12_NVAPI
  #include <d3d12.h>
  #include <dxgi1_6.h>
  #include <nvapi.h>
#endif

#if SPARKLE_RHI_WITH_D3D12_NVAPI
  #if defined(NVAPI_GET_BUILD_RAYTRACING_PARTITIONED_TLAS_INDIRECT_PREBUILD_INFO_PARAMS_VER) && defined(NVAPI_BUILD_RAYTRACING_PARTITIONED_TLAS_INDIRECT_PARAMS_VER)
	#define SPARKLE_RHI_D3D12_NVAPI_PACKS_PTLAS 1
  #else
	#define SPARKLE_RHI_D3D12_NVAPI_PACKS_PTLAS 0
  #endif
#else
  #define SPARKLE_RHI_D3D12_NVAPI_PACKS_PTLAS 0
#endif

#if SPARKLE_RHI_D3D12_NVAPI_PACKS_PTLAS
static std::uint32_t ToNvapiPtlasInstanceFlags(RhiPtlasInstanceFlags flags) noexcept
{
	std::uint32_t nativeFlags = NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_INSTANCE_FLAG_NONE;
	if (HasFlag(flags, RhiPtlasInstanceFlags::TriangleFacingCullDisable))
	{
		nativeFlags |= NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::TriangleFlipFacing))
	{
		nativeFlags |= NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::ForceOpaque))
	{
		nativeFlags |= NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_INSTANCE_FLAG_FORCE_OPAQUE;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::ForceNoOpaque))
	{
		nativeFlags |= NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_INSTANCE_FLAG_FORCE_NON_OPAQUE;
	}
	if (HasFlag(flags, RhiPtlasInstanceFlags::EnableExplicitBoundingBox))
	{
		nativeFlags |= NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_INSTANCE_FLAG_ENABLE_EXPLICIT_AABB;
	}
	return nativeFlags;
}

static std::uint32_t ToNvapiPtlasOperationType(ERhiPtlasOperationType type) noexcept
{
	switch (type)
	{
		case ERhiPtlasOperationType::UpdateInstance:
			return NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_UPDATE_INSTANCE;
		case ERhiPtlasOperationType::WritePartitionTranslation:
			return NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_WRITE_PARTITION;
		case ERhiPtlasOperationType::WriteInstance:
		default:
			return NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_WRITE_INSTANCE;
	}
}

#endif

RhiPtlasNativeOperationLayout D3D12PtlasOperationPacking::GetNativeLayout() noexcept
{
#if SPARKLE_RHI_D3D12_NVAPI_PACKS_PTLAS
	return RhiPtlasNativeOperationLayout{
	    .OperationCountSizeInBytes = sizeof(NvU32),
	    .OperationHeaderStrideInBytes = sizeof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP),
	    .OperationHeaderAlignmentInBytes = alignof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP),
	    .InstanceWriteStrideInBytes = sizeof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_INSTANCE),
	    .InstanceWriteAlignmentInBytes = alignof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_INSTANCE),
	    .InstanceUpdateStrideInBytes = sizeof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_UPDATE_INSTANCE),
	    .InstanceUpdateAlignmentInBytes = alignof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_UPDATE_INSTANCE),
	    .PartitionTranslationStrideInBytes = sizeof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_PARTITION),
	    .PartitionTranslationAlignmentInBytes = alignof(NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_PARTITION),
	    .BufferAlignmentInBytes = 16};
#else
	return {};
#endif
}

#if SPARKLE_RHI_D3D12_NVAPI_PACKS_PTLAS
static void PackOperations(
    const RhiPtlasOperationPackDesc& operationPack,
    NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP* nativeOperations,
    const RhiPtlasOperationBufferLayout& layout,
    RhiGpuVirtualAddress baseAddress)
{
	const RhiPtlasNativeOperationLayout nativeLayout = D3D12PtlasOperationPacking::GetNativeLayout();
	const RhiGpuVirtualAddress instanceWriteAddress = baseAddress + layout.InstanceWriteRecordsOffsetInBytes;
	const RhiGpuVirtualAddress instanceUpdateAddress = baseAddress + layout.InstanceUpdateRecordsOffsetInBytes;
	const RhiGpuVirtualAddress partitionTranslationAddress = baseAddress + layout.PartitionTranslationRecordsOffsetInBytes;
	for (std::uint32_t operationIndex = 0; operationIndex < operationPack.OperationCount; ++operationIndex)
	{
		const RhiPtlasOperationHeader& operation = operationPack.Operations[operationIndex];
		nativeOperations[operationIndex].type = static_cast<NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_TYPE>(ToNvapiPtlasOperationType(operation.Type));
		nativeOperations[operationIndex].count = operation.ArgumentCount;

		nativeOperations[operationIndex].data.StartAddress = RhiPtlasOperationLayout::ResolveArgumentAddress(operation, instanceWriteAddress, instanceUpdateAddress, partitionTranslationAddress);

		nativeOperations[operationIndex].data.StrideInBytes = RhiPtlasOperationLayout::ResolveArgumentStride(operation, nativeLayout);
	}
}

static void PackInstanceWrites(const RhiPtlasOperationPackDesc& operationPack, NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_INSTANCE* nativeInstanceWrites)
{
	for (std::uint32_t instanceIndex = 0; instanceIndex < operationPack.InstanceWriteCount; ++instanceIndex)
	{
		const RhiPtlasInstanceWriteDesc& source = operationPack.InstanceWrites[instanceIndex];
		NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_INSTANCE& target = nativeInstanceWrites[instanceIndex];
		for (std::uint32_t transformIndex = 0; transformIndex < source.Transform.size(); ++transformIndex)
		{
			target.transform[transformIndex / 4][transformIndex % 4] = source.Transform[transformIndex];
		}
		std::memcpy(target.userAABB, source.ExplicitBoundingBox.data(), sizeof(target.userAABB));
		target.instanceID = source.InstanceID;
		target.instanceMask = source.InstanceMask;
		target.instanceContributionToHitGroupIndex = source.InstanceContributionToHitGroupIndex;
		target.instanceFlags = ToNvapiPtlasInstanceFlags(source.Flags);
		target.instanceIndex = source.InstanceIndex;
		target.partitionIndex = source.PartitionIndex;
		target.accelerationStructure = source.AccelerationStructure;
	}
}

static void PackInstanceUpdates(const RhiPtlasOperationPackDesc& operationPack, NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_UPDATE_INSTANCE* nativeInstanceUpdates)
{
	for (std::uint32_t instanceIndex = 0; instanceIndex < operationPack.InstanceUpdateCount; ++instanceIndex)
	{
		const RhiPtlasInstanceUpdateDesc& source = operationPack.InstanceUpdates[instanceIndex];
		NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_UPDATE_INSTANCE& target = nativeInstanceUpdates[instanceIndex];
		target.instanceIndex = source.InstanceIndex;
		target.instanceContributionToHitGroupIndex = source.InstanceContributionToHitGroupIndex;
		target.accelerationStructure = source.AccelerationStructure;
	}
}

static void PackPartitionTranslations(const RhiPtlasOperationPackDesc& operationPack, NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_PARTITION* nativePartitionTranslations)
{
	for (std::uint32_t partitionIndex = 0; partitionIndex < operationPack.PartitionTranslationCount; ++partitionIndex)
	{
		const RhiPtlasPartitionTranslationDesc& source = operationPack.PartitionTranslations[partitionIndex];
		NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_PARTITION& target = nativePartitionTranslations[partitionIndex];
		target.partitionIndex = source.PartitionIndex;
		target.partitionTranslation[0] = source.Translation[0];
		target.partitionTranslation[1] = source.Translation[1];
		target.partitionTranslation[2] = source.Translation[2];
	}
}

#endif

std::vector<std::uint8_t> D3D12PtlasOperationPacking::Pack(const RhiPtlasOperationPackDesc& operationPack, const RhiPtlasOperationBufferLayout& layout, RhiGpuVirtualAddress baseAddress)
{
#if SPARKLE_RHI_D3D12_NVAPI_PACKS_PTLAS
	std::vector<std::uint8_t> packed(static_cast<std::size_t>(layout.TotalSizeInBytes), 0);
	std::memcpy(packed.data() + layout.OperationCountOffsetInBytes, &operationPack.OperationCount, sizeof(operationPack.OperationCount));

	auto* const nativeOperations = reinterpret_cast<NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP*>(packed.data() + static_cast<std::size_t>(layout.OperationHeadersOffsetInBytes));

	auto* const nativeInstanceWrites = reinterpret_cast<NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_INSTANCE*>(
	    packed.data() + static_cast<std::size_t>(layout.InstanceWriteRecordsOffsetInBytes));

	auto* const nativeInstanceUpdates = reinterpret_cast<NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_UPDATE_INSTANCE*>(
	    packed.data() + static_cast<std::size_t>(layout.InstanceUpdateRecordsOffsetInBytes));

	auto* const nativePartitionTranslations = reinterpret_cast<NVAPI_D3D12_BUILD_RAYTRACING_PARTITIONED_TLAS_OP_ARG_WRITE_PARTITION*>(
	    packed.data() + static_cast<std::size_t>(layout.PartitionTranslationRecordsOffsetInBytes));

	PackOperations(operationPack, nativeOperations, layout, baseAddress);
	PackInstanceWrites(operationPack, nativeInstanceWrites);
	PackInstanceUpdates(operationPack, nativeInstanceUpdates);
	PackPartitionTranslations(operationPack, nativePartitionTranslations);
	return packed;
#else
	static_cast<void>(operationPack);
	static_cast<void>(layout);
	static_cast<void>(baseAddress);
	return {};
#endif
}
