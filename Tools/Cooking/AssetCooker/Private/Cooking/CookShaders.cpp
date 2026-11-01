#include "Cooking/AssetCookStages.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Dispatch/AssetCookerToolProcess.h"
#include "Core/Public/Paths/FileSystemPaths.h"

bool ValidateShaderCookStage(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics)
{
	std::error_code error;
	if (!std::filesystem::is_regular_file(plan.shaderCompilerPath, error) || error)
	{
		diagnostics.AddError(AssetCookerCategory::Shaders, "ShaderCompiler executable was not found.", plan.shaderCompilerPath);
		return false;
	}
	return true;
}

bool CookShaders(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outputs)
{
	if (AssetCookerToolProcess::Run(plan.shaderCompilerPath, {"cook", "--debug-info"}, plan.projectRoot) != 0)
	{
		diagnostics.AddError(AssetCookerCategory::Shaders, "Shader cooking failed.");
		return false;
	}

	const std::filesystem::path cookedShaderRoot = plan.cookedRoot / "Shaders";
	outputs.push_back({AssetCookerCategory::Shaders, "global-shader-map", Strings::ToNarrow((Filesystem::BuildGlobalShaderMapPath(cookedShaderRoot)).wstring())});
	outputs.push_back({AssetCookerCategory::Shaders, "cooked-shader-library", Strings::ToNarrow((Filesystem::BuildCookedShaderLibraryPath(cookedShaderRoot)).wstring())});
	return true;
}
