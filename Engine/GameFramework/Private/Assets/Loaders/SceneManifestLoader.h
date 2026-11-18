#pragma once

#include <span>
#include <cstdint>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedSceneManifest;

	namespace SceneManifestLoader
	{
		LoadedSceneManifest Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
