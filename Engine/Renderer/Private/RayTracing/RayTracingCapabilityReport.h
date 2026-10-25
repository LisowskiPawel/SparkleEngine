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
	bool Supported = false;
	bool SupportsDescriptor = false;
	const char* CapabilityStatusReason = "not-queried";
};

struct RayTracingCapabilityReport final
{
	bool SupportsAccelerationStructure = false;
	bool SupportsInlineRayQuery = false;
	bool SupportsRayTracingPipeline = false;
	bool SupportsDescriptorIndexing = false;
	RayTracingTlasProviderCapabilityReport TlasProvider;
	RayTracingPtlasCapabilityReport Ptlas;
};

RayTracingCapabilityReport BuildRayTracingCapabilityReport(const RhiCapabilities& capabilities) noexcept;
