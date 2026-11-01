#include "Cooking/AssetCookStages.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Cooking/AssetCookerSceneBatch.h"
#include "ToolWorkProgress.h"

#include <iostream>

bool CookSceneAssets(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outputs)
{
	ToolWorkProgressWriter progress(std::cout);
	if (!AssetCookerSceneBatch::Execute(plan.sceneEntries, diagnostics, [&progress](const ToolWorkProgressEvent& event) { progress.Report(event); }))
	{
		return false;
	}

	outputs.push_back({AssetCookerCategory::SceneAssets, "scene-manifests", Strings::ToNarrow((plan.cookedRoot / "SceneManifests").wstring())});
	outputs.push_back({AssetCookerCategory::Meshes, "meshes", Strings::ToNarrow((plan.cookedRoot / "Meshes").wstring())});
	outputs.push_back({AssetCookerCategory::Materials, "materials", Strings::ToNarrow((plan.cookedRoot / "Materials").wstring())});
	return true;
}
