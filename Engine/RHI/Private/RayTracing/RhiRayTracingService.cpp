#include "PCH.h"

#include "RayTracing/RhiRayTracingService.h"

RhiRayTracingAccelerationStructurePrebuildInfo RhiRayTracingService::GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags) const noexcept
{
	return GetTlasService().GetTlasPrebuildInfo(instanceCount, buildFlags);
}

RhiPtlasBuildSizes RhiRayTracingService::GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept
{
	return GetPtlasService().GetPtlasBuildSizes(desc);
}

RhiOwnedResourceHandle RhiRayTracingService::CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName)
{
	return GetPtlasService().CreatePtlasBuffer(sizes, debugName);
}

RhiOwnedResourceHandle RhiRayTracingService::CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName)
{
	return GetPtlasService().CreatePtlasOperationBuffer(operationPack, debugName);
}

RhiPtlasOperationBufferLayout RhiRayTracingService::GetPtlasOperationBufferLayout(const RhiPtlasDesc& desc) const noexcept
{
	return GetPtlasService().GetPtlasOperationBufferLayout(desc);
}

RhiOwnedResourceHandle RhiRayTracingService::CreateRayTracingInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName)
{
	return GetTlasService().CreateTlasInstanceBuffer(instances, instanceCount, debugName);
}
