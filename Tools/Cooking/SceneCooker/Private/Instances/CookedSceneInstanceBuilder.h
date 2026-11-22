#pragma once

#include "CookedSceneBuild.h"
#include "SourceImportOutput.h"

namespace CookedSceneInstanceBuilder
{
	void BuildInstances(const SourceImportOutput& importOutput, CookedSceneBuild& build);
}
