#pragma once

#include "RHI/Public/RayTracing/RhiRayTracingDesc.h"

struct RhiCapabilities;

struct RayTracingTlasProviderCapabilityReport final
{
	ERhiRayTracingTlasProvider SelectedProvider = ERhiRayTracingTlasProvider::None;
	const char* SelectionReason = "not-queried";
};

struct RayTracingPtlasCapabilityReport final
{
	ERhiPtlasProvider Provider = ERhiPtlasProvider::None;
	bool bSupported = false;
	bool bSupportsDescriptor = false;
	const char* CapabilityStatusReason = "not-queried";
};

struct RayTracingCapabilityReport final
{
	bool bSupportsAccelerationStructure = false;
	bool bSupportsInlineRayQuery = false;
	bool bSupportsRayTracingPipeline = false;
	bool bSupportsDescriptorIndexing = false;
	RayTracingTlasProviderCapabilityReport TlasProvider;
	RayTracingPtlasCapabilityReport Ptlas;
};

RayTracingCapabilityReport BuildRayTracingCapabilityReport(const RhiCapabilities& capabilities) noexcept;
