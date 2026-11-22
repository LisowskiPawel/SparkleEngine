#pragma once

#include "GameFramework/Public/Assets/Cooked/CookedAssetCommon.h"

#include <cstdint>
#include <span>

struct AnimationClipResource;
struct SkeletonResource;

namespace Assets
{
	class CookedAssetAllocationBudget;

	namespace AnimationAssetLoader
	{
		AnimationClipResource Load(std::span<const std::uint8_t> bytes, CookedAssetId animationAssetId, std::span<const SkeletonResource> skeletons, CookedAssetAllocationBudget& budget);
	}
}
