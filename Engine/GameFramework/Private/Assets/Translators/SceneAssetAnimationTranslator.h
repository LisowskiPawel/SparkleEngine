#pragma once

#include "Assets/Cooked/LoadedAnimationAsset.h"
#include "Animation/AnimationClipResource.h"
#include "Animation/SkeletonResource.h"

namespace Assets
{
	class CookedAssetAllocationBudget;

	AnimationClipResource BuildSceneAssetAnimation(const LoadedAnimationAsset& animationAsset, CookedAssetId animationAssetId, const SkeletonResource& skeleton, CookedAssetAllocationBudget& budget);
}
