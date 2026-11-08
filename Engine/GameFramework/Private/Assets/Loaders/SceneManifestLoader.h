#pragma once

#include <filesystem>
#include <span>
#include <cstdint>

namespace Assets
{
	class CookedAssetAllocationBudget;
	struct LoadedSceneManifest;

	namespace SceneManifestLoader
	{
		LoadedSceneManifest Decode(const std::filesystem::path& path, std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget);
	}
}
