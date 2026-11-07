#include "PCH.h"

#include "DxcCompileArguments.h"

#include "Compiler/ShaderCompileRequest.h"
#include "Compiler/ShaderCompileProfile.h"
#include "Core/Public/Strings/StringUtils.h"

#include <ObjIdl.h>
#include <dxcapi.h>

static void AppendSourceAndEntryPoint(
    const ShaderCompileRequest& request,
    const std::wstring& wSourcePath,
    const std::wstring& wEntryPoint,
    const std::wstring& wTargetProfile,
    std::vector<const wchar_t*>& outArgs)
{
	outArgs.push_back(wSourcePath.c_str());

	if (request.UnitKind != ShaderCompileUnitKind::Library)
	{
		outArgs.push_back(L"-E");
		outArgs.push_back(wEntryPoint.c_str());
	}
	else
	{
		outArgs.push_back(L"-exports");
		outArgs.push_back(wEntryPoint.c_str());
	}

	outArgs.push_back(L"-T");
	outArgs.push_back(wTargetProfile.c_str());

	outArgs.push_back(L"-HV");
	outArgs.push_back(L"2021");
}

static void AppendDefines(const std::vector<std::string>& defines, std::vector<std::wstring>& wDefines, std::vector<const wchar_t*>& outArgs)
{
	wDefines.clear();
	for (const auto& def : defines)
	{
		wDefines.push_back(Strings::ToWide(std::string_view{def}));
	}
	for (const auto& def : wDefines)
	{
		outArgs.push_back(L"-D");
		outArgs.push_back(def.c_str());
	}
}

static void AppendArtifactOptions(const ShaderCompileRequest& request, std::vector<const wchar_t*>& outArgs)
{
	outArgs.push_back(DXC_ARG_ENABLE_STRICTNESS);
	outArgs.push_back(DXC_ARG_ALL_RESOURCES_BOUND);

	if (request.UnitKind == ShaderCompileUnitKind::Library && !IsSpirVTarget(request.Target))
	{
		outArgs.push_back(L"-auto-binding-space");
		outArgs.push_back(L"0");
	}

	if (request.bTreatWarningsAsErrors)
	{
		outArgs.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
	}

	// Cooked shader artifacts serialize typed reflection separately, so runtime DXIL
	// does not need the embedded reflection payload. Both flags are DXIL-only.
	if (!IsSpirVTarget(request.Target))
	{
		outArgs.push_back(L"-Qstrip_reflect");
		if (request.bStripDebugInfo)
		{
			outArgs.push_back(L"-Qstrip_debug");
		}
	}

	if (request.bEnableDebugInfo)
	{
		outArgs.push_back(DXC_ARG_DEBUG);
		outArgs.push_back(IsSpirVTarget(request.Target) ? L"-fspv-debug=vulkan-with-source" : L"-Qsource_in_debug_module");
	}

	if (request.bEnableOptimizations)
	{
		outArgs.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
	}
	else
	{
		outArgs.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
	}
}

static void AppendSpirVOptions(const ShaderCompileRequest& request, std::vector<const wchar_t*>& outArgs)
{
	// SPIR-V mode toggle. DXC emits SPIR-V when -spirv is present; the same
	// HLSL profile string still selects the shader stage.
	if (IsSpirVTarget(request.Target))
	{
		outArgs.push_back(L"-spirv");
		if constexpr (bSpirVUseDirectXBufferLayout)
		{
			outArgs.push_back(L"-fvk-use-dx-layout");
		}
		if constexpr (bSpirVUseUnknownStorageImageFormat)
		{
			outArgs.push_back(L"-fspv-use-unknown-image-format");
		}
		switch (request.Target)
		{
			case ShaderTarget::SpirV14:
				outArgs.push_back(L"-fspv-target-env=vulkan1.1spirv1.4");
				break;
			case ShaderTarget::SpirV15:
				outArgs.push_back(L"-fspv-target-env=vulkan1.2");
				break;
			case ShaderTarget::SpirV16:
				outArgs.push_back(L"-fspv-target-env=vulkan1.3");
				break;
			default:
				break;
		}
		if (HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::InlineRayQuery))
		{
			outArgs.push_back(L"-fspv-extension=SPV_KHR_ray_query");
		}

		const bool bRequiresRayTracingExtension = HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::AccelerationStructure)
		    || HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::RayTracingPipeline);

		if (bRequiresRayTracingExtension)
		{
			outArgs.push_back(L"-fspv-extension=SPV_KHR_ray_tracing");
		}
		if (HasShaderCompileFeature(request.RequiredFeatures, ShaderCompileFeatureFlags::DescriptorIndexing))
		{
			outArgs.push_back(L"-fspv-extension=SPV_EXT_descriptor_indexing");
		}
	}
}

void BuildDxcCompileArguments(
    const ShaderCompileRequest& request,
    const std::wstring& wSourcePath,
    const std::wstring& wEntryPoint,
    const std::wstring& wTargetProfile,
    std::vector<std::wstring>& wDefines,
    std::vector<const wchar_t*>& outArgs)
{
	outArgs.clear();
	outArgs.reserve(32);

	AppendSourceAndEntryPoint(request, wSourcePath, wEntryPoint, wTargetProfile, outArgs);
	AppendDefines(request.Defines, wDefines, outArgs);
	AppendArtifactOptions(request, outArgs);
	AppendSpirVOptions(request, outArgs);
}
