#include "Cooking/AssetCookStages.h"
#include "Dispatch/AssetCookerToolProcess.h"

bool ValidateShaderCookStage(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics)
{
	std::error_code error;
	if (!std::filesystem::is_regular_file(plan.shaderCompilerPath, error) || error)
	{
		diagnostics.AddError("ShaderCompiler executable was not found.", plan.shaderCompilerPath);
		return false;
	}
	return true;
}

bool CookShaders(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics)
{
	if (AssetCookerToolProcess::Run(plan.shaderCompilerPath, {"cook"}, plan.projectRoot) != 0)
	{
		diagnostics.AddError("Shader cooking failed.");
		return false;
	}

	return true;
}
