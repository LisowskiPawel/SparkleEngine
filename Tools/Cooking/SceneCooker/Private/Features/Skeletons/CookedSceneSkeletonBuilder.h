#pragma once

#include "CookedSceneBuild.h"
#include "SourceImportOutput.h"

#include <string_view>

namespace CookedSceneSkeletonBuilder
{
	void BuildSkeletons(const SourceImportOutput& importOutput, std::string_view sceneAssetId, CookedSceneBuild& outBuild);
}
