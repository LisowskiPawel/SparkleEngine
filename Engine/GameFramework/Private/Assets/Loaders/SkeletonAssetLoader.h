#pragma once

#include <filesystem>
#include <cstdint>
#include <span>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedSkeletonAsset;

	namespace SkeletonAssetLoader
	{
		LoadedSkeletonAsset Decode(const std::filesystem::path& path, std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
