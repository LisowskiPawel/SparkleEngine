#pragma once

#include <filesystem>
#include <span>
#include <cstdint>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedMaterialAsset;

	namespace MaterialAssetLoader
	{
		LoadedMaterialAsset Decode(const std::filesystem::path& path, std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
