#pragma once

#include "CookedSceneBuild.h"
#include "SourceImportOutput.h"

namespace CookedSceneCameraBuilder
{
	void BuildCameras(const SourceImportOutput& importOutput, CookedSceneBuild& outBuild);
}
