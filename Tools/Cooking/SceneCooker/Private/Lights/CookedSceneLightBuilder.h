#pragma once

#include "CookedSceneBuild.h"
#include "SourceImportOutput.h"

namespace CookedSceneLightBuilder
{
	void BuildLights(const SourceImportOutput& importOutput, CookedSceneBuild& outBuild);
}
