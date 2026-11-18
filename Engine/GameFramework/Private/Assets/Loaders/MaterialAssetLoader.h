#pragma once

#include <span>
#include <cstdint>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedMaterialAsset;

	namespace MaterialAssetLoader
	{
		LoadedMaterialAsset Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
