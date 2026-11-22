#pragma once

#include "RHI/Public/Shaders/ShaderTarget.h"

struct ShaderCompileRequest;

// Target and feature support used to admit requests during planning.
struct ShaderBackendCapabilities final
{
	bool SupportsTarget(ShaderTarget target) const noexcept;
	bool SupportsRayTracingLibrary(ShaderTarget target) const noexcept;
	bool SupportsInlineRayQuery(ShaderTarget target) const noexcept;
	void ValidateRequest(const ShaderCompileRequest& request) const;

	bool bSupportsDxil = false;
	bool bSupportsSpirV = false;
	bool bSupportsDxilRayTracingLibrary = false;
	bool bSupportsSpirVRayTracingLibrary = false;
	bool bSupportsDxilInlineRayQuery = false;
	bool bSupportsSpirVInlineRayQuery = false;
};
