#pragma once

#include "Assets/Cooked/CookedMeshAsset.h"

#include <cstdint>
#include <span>
#include <memory>

class Mesh;

namespace Assets
{
	class CookedAssetAllocationBudget;

	namespace MeshAssetLoader
	{
		std::shared_ptr<const Mesh> Decode(std::span<const std::uint8_t> bytes, CookedAssetId assetId, CookedMeshAssetKind expectedKind, CookedAssetAllocationBudget& budget);
	}
}
