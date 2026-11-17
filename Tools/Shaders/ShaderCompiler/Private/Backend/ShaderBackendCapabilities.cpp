#include "PCH.h"
#include "Backend/ShaderBackendCapabilities.h"

bool ShaderBackendCapabilities::SupportsTarget(ShaderTarget target) const noexcept
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

bool ShaderBackendCapabilities::SupportsRayTracingLibrary(ShaderTarget target) const noexcept
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

bool ShaderBackendCapabilities::SupportsInlineRayQuery(ShaderTarget target) const noexcept
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
