#include "../PCH.h"
#include "RayTracing/RayTracingCapabilityReport.h"

#include "RHI/Public/Core/RhiCapabilities.h"

RayTracingCapabilityReport BuildRayTracingCapabilityReport(const RhiCapabilities& capabilities) noexcept
{
	const RhiRayTracingCapabilities& rayTracing = capabilities.RayTracing;
	const bool supportsPtlasDescriptor = rayTracing.Groups.Ptlas.SupportsDescriptorAccess;

	return RayTracingCapabilityReport{
	    .SupportsAccelerationStructure = rayTracing.SupportsAccelerationStructure,
	    .SupportsInlineRayQuery = rayTracing.SupportsInlineRayQuery,
	    .SupportsRayTracingPipeline = rayTracing.SupportsRayTracingPipeline,
	    .SupportsDescriptorIndexing = capabilities.DescriptorIndexing.SupportsSampledImageArrayNonUniformIndexing && capabilities.DescriptorIndexing.SupportsPartiallyBoundDescriptorArrays,
	    .TlasProvider =
	        RayTracingTlasProviderCapabilityReport{.SelectedProvider = rayTracing.Groups.Provider.SelectedTlasProvider, .SelectionReason = rayTracing.Groups.Provider.SelectedTlasProviderReason},
	    .Ptlas = RayTracingPtlasCapabilityReport{
	        .Provider = rayTracing.Groups.Ptlas.Provider,
	        .Supported = rayTracing.Groups.Ptlas.Supported,
	        .SupportsDescriptor = supportsPtlasDescriptor,
	        .CapabilityStatusReason = rayTracing.Groups.Ptlas.CapabilityStatusReason}};
}
