#pragma once

#include "Assets/Cooked/LoadedMeshAsset.h"

#include <cstdint>
#include <span>

namespace Assets
{
	class CookedAssetAllocationBudget;

	namespace MeshAssetLoader
	{
		LoadedMeshAsset Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
