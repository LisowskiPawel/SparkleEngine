#include "PCH.h"

#include "RayTracing/RhiRayTracingDesc.h"

void PopulateStandardRayTracingCapabilityGroups(RhiRayTracingCapabilities& capabilities) noexcept
{
	capabilities.Groups.Tlas = RhiTlasCapabilities{
	    .bSupportsTlasBuild = capabilities.bSupportsAccelerationStructure,
	    .bSupportsTlasUpdate = capabilities.bSupportsAccelerationStructure,
	    .bSupportsGpuReadableInstanceBuffer = capabilities.bSupportsAccelerationStructure,
	    .InstanceDescSizeInBytes = capabilities.InstanceDescSizeInBytes};
}

const char* RhiRayTracingTlasProviderToString(ERhiRayTracingTlasProvider provider) noexcept
{
	switch (provider)
	{
		case ERhiRayTracingTlasProvider::Tlas:
			return "TLAS";
		case ERhiRayTracingTlasProvider::Ptlas:
			return "PTLAS";
		case ERhiRayTracingTlasProvider::None:
		default:
			return "None";
	}
}
