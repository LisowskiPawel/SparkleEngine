#pragma once

#include "SourceImportOutput.h"
#include "GameFramework/Public/Assets/Cooked/CookedAnimationAsset.h"

#include <vector>

namespace AnimationPreviewBaker
{
	std::vector<Assets::CookedAnimationKeyframeRecord> Bake(const ImportedAnimationSampler& sampler, ImportedAnimationTargetPath path, float durationSeconds);
}
