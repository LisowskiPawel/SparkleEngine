#include "PCH.h"
#include "Backend/ShaderBackendCapabilities.h"

#include "Compiler/ShaderCompileRequest.h"
#include "Core/Public/Diagnostics/Error.h"

#include <string>

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

void ShaderBackendCapabilities::ValidateRequest(const ShaderCompileRequest& request) const
{
	if (!SupportsTarget(request.Target))
	{
		throw Diagnostics::Error(std::string("Active shader backend does not support target '") + GetShaderTargetName(request.Target) + "'.");
	}
	const bool bRayLibraryRequired = HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::RayTracingPipeline);
	const bool bRayLibraryUnsupported = bRayLibraryRequired && !SupportsRayTracingLibrary(request.Target);
	if (bRayLibraryUnsupported)
	{
		throw Diagnostics::Error(std::string("Active shader backend does not support shader libraries for target '") + GetShaderTargetName(request.Target) + "'.");
	}
	const bool bInlineQueryRequired = HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::InlineRayQuery);
	const bool bInlineQueryUnsupported = bInlineQueryRequired && !SupportsInlineRayQuery(request.Target);
	if (bInlineQueryUnsupported)
	{
		throw Diagnostics::Error(std::string("Active shader backend does not support inline ray queries for target '") + GetShaderTargetName(request.Target) + "'.");
	}
}
