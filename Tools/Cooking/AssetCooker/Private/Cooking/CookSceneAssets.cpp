#include "Cooking/AssetCookStages.h"
#include "Cooking/AssetCookerSceneBatch.h"
#include "ToolWorkProgressWriter.h"

#include <iostream>

bool CookSceneAssets(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics)
{
	ToolWorkProgressWriter progress(std::cout);

	const auto reportProgress = [&progress](const ToolWorkProgressEvent& event)
	{
		progress.Report(event);
	};

	return AssetCookerSceneBatch::Execute(plan.sceneEntries, diagnostics, reportProgress);
}
