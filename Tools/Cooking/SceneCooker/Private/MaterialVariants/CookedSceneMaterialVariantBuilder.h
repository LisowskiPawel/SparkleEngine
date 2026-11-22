#pragma once

#include "CookedSceneBuild.h"
#include "SourceImportOutput.h"

namespace CookedSceneMaterialVariantBuilder
{
	void BuildMaterialVariants(const SourceImportOutput& importOutput, CookedSceneBuild& outBuild);
}
