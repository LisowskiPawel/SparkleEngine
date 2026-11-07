#include "../PCH.h"
#include "RayTracing/RayTracingCapabilityReport.h"

#include "RHI/Public/Core/RhiCapabilities.h"

RayTracingCapabilityReport BuildRayTracingCapabilityReport(const RhiCapabilities& capabilities) noexcept
{
	const RhiRayTracingCapabilities& rayTracing = capabilities.RayTracing;
	const bool bSupportsPtlasDescriptor = rayTracing.Groups.Ptlas.bSupportsDescriptorAccess;

	return RayTracingCapabilityReport{
	    .bSupportsAccelerationStructure = rayTracing.bSupportsAccelerationStructure,
	    .bSupportsInlineRayQuery = rayTracing.bSupportsInlineRayQuery,
	    .bSupportsRayTracingPipeline = rayTracing.bSupportsRayTracingPipeline,
	    .bSupportsDescriptorIndexing = capabilities.DescriptorIndexing.bNonUniformSampledImages && capabilities.DescriptorIndexing.bSupportsPartiallyBoundDescriptorArrays,
	    .TlasProvider =
	        RayTracingTlasProviderCapabilityReport{.SelectedProvider = rayTracing.Groups.Provider.SelectedTlasProvider, .SelectionReason = rayTracing.Groups.Provider.SelectedTlasProviderReason},
	    .Ptlas = RayTracingPtlasCapabilityReport{
	        .Provider = rayTracing.Groups.Ptlas.Provider,
	        .bSupported = rayTracing.Groups.Ptlas.bSupported,
	        .bSupportsDescriptor = bSupportsPtlasDescriptor,
	        .CapabilityStatusReason = rayTracing.Groups.Ptlas.CapabilityStatusReason}};
}
