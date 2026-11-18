#pragma once

#include <cstdint>
#include <span>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedSkeletonAsset;

	namespace SkeletonAssetLoader
	{
		LoadedSkeletonAsset Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
