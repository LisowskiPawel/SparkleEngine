#pragma once

#include <cstdint>
#include <filesystem>
#include <span>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedAnimationAsset;

	namespace AnimationAssetLoader
	{
		LoadedAnimationAsset Decode(const std::filesystem::path& path, std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
