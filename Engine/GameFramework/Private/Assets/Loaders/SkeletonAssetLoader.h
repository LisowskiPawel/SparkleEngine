#pragma once

#include "GameFramework/Public/Assets/Cooked/CookedAssetCommon.h"

#include <cstdint>
#include <span>

struct SkeletonResource;

namespace Assets
{
	class CookedAssetAllocationBudget;

	namespace SkeletonAssetLoader
	{
		SkeletonResource Load(std::span<const std::uint8_t> bytes, CookedAssetId skeletonAssetId, CookedAssetAllocationBudget& budget);
	}
}
