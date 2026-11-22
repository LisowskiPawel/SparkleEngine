#include "PCH.h"

#include "Compiler/ShaderCompileProfile.h"

#include "RHI/Public/Shaders/ShaderStage.h"
#include "Core/Public/Diagnostics/Error.h"

#include <string>
#include <string_view>

const char* GetShaderModelProfileName(ShaderTarget target)
{
	switch (target)
	{
		case ShaderTarget::DxilSm60:
			return "sm_6_0";
		case ShaderTarget::DxilSm61:
			return "sm_6_1";
		case ShaderTarget::DxilSm62:
			return "sm_6_2";
		case ShaderTarget::DxilSm63:
			return "sm_6_3";
		case ShaderTarget::DxilSm64:
			return "sm_6_4";
		case ShaderTarget::DxilSm65:
			return "sm_6_5";
		case ShaderTarget::DxilSm66:
			return "sm_6_6";
		case ShaderTarget::DxilSm67:
			return "sm_6_7";
		default:
			return "sm_6_6";
	}
}

const char* GetSpirVProfileName(ShaderTarget target)
{
	switch (target)
	{
		case ShaderTarget::SpirV14:
			return "spirv_1_4";
		case ShaderTarget::SpirV15:
			return "spirv_1_5";
		case ShaderTarget::SpirV16:
			return "spirv_1_6";
		default:
			return "unknown";
	}
}

const char* GetSlangTargetProfileName(ShaderTarget target)
{
	return IsSpirVTarget(target) ? GetSpirVProfileName(target) : GetShaderModelProfileName(target);
}

const char* GetSlangShaderStageName(ShaderStage stage)
{
	switch (stage)
	{
		case ShaderStage::Vertex:
			return "vertex";
		case ShaderStage::Pixel:
			return "fragment";
		case ShaderStage::Geometry:
			return "geometry";
		case ShaderStage::Hull:
			return "hull";
		case ShaderStage::Domain:
			return "domain";
		case ShaderStage::Compute:
			return "compute";
		case ShaderStage::RayGeneration:
			return "raygeneration";
		case ShaderStage::Miss:
			return "miss";
		case ShaderStage::ClosestHit:
			return "closesthit";
		case ShaderStage::AnyHit:
			return "anyhit";
		case ShaderStage::Intersection:
			return "intersection";
		case ShaderStage::Callable:
			return "callable";
		default:
			throw Diagnostics::Error("Unsupported Slang shader stage.");
	}
}

std::string BuildShaderTargetProfile(const ShaderCompileRequest& request)
{
	std::string profile;
	profile.reserve(8);
	profile += request.UnitKind == ShaderCompileUnitKind::Library ? "lib" : GetShaderStagePrefix(request.Stage);
	profile += '_';
	const std::string_view shaderModel = GetShaderModelProfileName(request.Target);
	profile += shaderModel.substr(3);
	return profile;
}
