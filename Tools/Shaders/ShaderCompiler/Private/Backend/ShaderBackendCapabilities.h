#pragma once

#include "RHI/Public/Shaders/ShaderTarget.h"

// What a backend can produce.
// The orchestrator picks the first backend that supports the requested target.
struct ShaderBackendCapabilities final
{
	bool SupportsTarget(ShaderTarget target) const noexcept;
	bool SupportsRayTracingLibrary(ShaderTarget target) const noexcept;
	bool SupportsInlineRayQuery(ShaderTarget target) const noexcept;

	bool bSupportsDxil = false;
	bool bSupportsSpirV = false;
	bool bSupportsDxilRayTracingLibrary = false;
	bool bSupportsSpirVRayTracingLibrary = false;
	bool bSupportsDxilInlineRayQuery = false;
	bool bSupportsSpirVInlineRayQuery = false;
};
