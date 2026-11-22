#pragma once

#include "Assets/Cooked/LoadedSkeletonAsset.h"
#include "Animation/SkeletonResource.h"

#include <cstdint>

namespace Assets
{
	class CookedAssetAllocationBudget;

	SkeletonResource BuildSceneAssetSkeleton(const LoadedSkeletonAsset& skeletonAsset, CookedAssetId skeletonAssetId, std::uint32_t sourceSkinIndex, CookedAssetAllocationBudget& budget);
}
