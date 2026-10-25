#include "PCH.h"

#include "RayTracing/RhiRayTracingDesc.h"

void PopulateStandardRayTracingCapabilityGroups(RhiRayTracingCapabilities& capabilities) noexcept
{
	capabilities.Groups.Tlas = RhiTlasCapabilities{
	    .SupportsTlasBuild = capabilities.SupportsAccelerationStructure,
	    .SupportsTlasUpdate = capabilities.SupportsAccelerationStructure,
	    .SupportsGpuReadableInstanceBuffer = capabilities.SupportsAccelerationStructure,
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
