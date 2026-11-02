#include "AssetCookerDispatcher.h"

#include "Cooking/AssetCookStages.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "ToolWorkProgress.h"

#include <array>
#include <iostream>
#include <string>

struct AssetCookStage final
{
	const char* Name;
	bool (*Validate)(const AssetCookerProjectCookPlan&, AssetCookerDiagnostics&);
	bool (*Cook)(const AssetCookerProjectCookPlan&, AssetCookerDiagnostics&, std::vector<AssetCookerOutputRecord>&);
};

static constexpr std::array Stages{
    AssetCookStage{"shaders", ValidateShaderCookStage, CookShaders},
    AssetCookStage{"textures", ValidateTextureCookStage, CookTextures},
    AssetCookStage{"scene-assets", nullptr, CookSceneAssets}};

bool DispatchAssetCookPlan(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outOutputs)
{
	for (const AssetCookerPlanStep step : plan.steps)
	{
		const auto index = static_cast<std::size_t>(step);
		if (index >= Stages.size())
		{
			diagnostics.AddError(AssetCookerCategory::All, "Unknown asset cook stage.");
			return false;
		}
		if (Stages[index].Validate && !Stages[index].Validate(plan, diagnostics))
		{
			return false;
		}
	}

	Filesystem::ConfigureProjectRoot(plan.projectRoot);
	ToolWorkProgressWriter progress(std::cout);
	for (std::size_t index = 0; index < plan.steps.size(); ++index)
	{
		const AssetCookStage& stage = Stages[static_cast<std::size_t>(plan.steps[index])];
		const std::string phase = "Asset cook stage: " + std::string(stage.Name);
		progress.Report(phase, index, plan.steps.size());
		if (!stage.Cook(plan, diagnostics, outOutputs))
		{
			return false;
		}
		progress.Report(phase, index + 1u, plan.steps.size());
	}
	return true;
}
