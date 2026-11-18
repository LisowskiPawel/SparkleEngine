#pragma once

#include <cstdint>
#include <span>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedAnimationAsset;

	namespace AnimationAssetLoader
	{
		LoadedAnimationAsset Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
