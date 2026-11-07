#pragma once

#include "RHI/Public/Shaders/ShaderTarget.h"

// What a backend can produce.
// The orchestrator picks the first backend that supports the requested target.
struct ShaderBackendCapabilities final
{
	bool SupportsTarget(ShaderTarget target) const noexcept
	{
		if (IsDxilTarget(target))
		{
			return bSupportsDxil;
		}
		if (IsSpirVTarget(target))
		{
			return bSupportsSpirV;
		}
		return false;
	}

	bool SupportsRayTracingLibrary(ShaderTarget target) const noexcept
	{
		if (IsDxilTarget(target))
		{
			return bSupportsDxilRayTracingLibrary;
		}
		if (IsSpirVTarget(target))
		{
			return bSupportsSpirVRayTracingLibrary;
		}
		return false;
	}

	bool SupportsInlineRayQuery(ShaderTarget target) const noexcept
	{
		if (IsDxilTarget(target))
		{
			return bSupportsDxilInlineRayQuery;
		}
		if (IsSpirVTarget(target))
		{
			return bSupportsSpirVInlineRayQuery;
		}
		return false;
	}

	bool bSupportsDxil = false;
	bool bSupportsSpirV = false;
	bool bSupportsDxilRayTracingLibrary = false;
	bool bSupportsSpirVRayTracingLibrary = false;
	bool bSupportsDxilInlineRayQuery = false;
	bool bSupportsSpirVInlineRayQuery = false;
};
