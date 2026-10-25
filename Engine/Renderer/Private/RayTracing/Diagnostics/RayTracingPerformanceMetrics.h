#pragma once

#include "RayTracing/Diagnostics/RayTracingBlasMetrics.h"
#include "RHI/Public/RayTracing/RhiRayTracingDesc.h"

#include <cstdint>

struct RayTracingProviderMetrics final
{
	ERhiRayTracingTlasProvider TlasProvider = ERhiRayTracingTlasProvider::None;
	const char* TlasProviderReason = "not-queried";
	ERhiPtlasProvider PtlasProvider = ERhiPtlasProvider::None;
	bool SupportsPtlas = false;
	const char* PtlasCapabilityReason = "not-queried";
};

struct RayTracingPerformanceMetrics final
{
	RayTracingProviderMetrics Providers;
	RayTracingBlasMetrics Blas;
	std::uint32_t TopLevelInstanceCount = 0;
};
