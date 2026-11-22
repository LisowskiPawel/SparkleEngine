#pragma once

#include "Assets/Cooked/LoadedSkeletonAsset.h"
#include "Animation/SkeletonResource.h"

namespace Assets
{
	class CookedAssetAllocationBudget;

	SkeletonResource BuildSceneAssetSkeleton(const LoadedSkeletonAsset& skeletonAsset, CookedAssetId skeletonAssetId, CookedAssetAllocationBudget& budget);
}
