#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace Filesystem
{
	// Cooked product paths
	SPARKLE_CORE_API const std::filesystem::path& GetCookedAssetRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedShaderRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetGlobalShaderMapPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedShaderLibraryPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedTextureRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedSceneManifestRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedMeshRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedMaterialRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedSkeletonRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetCookedAnimationRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetSceneAssetRegistryPath();
	SPARKLE_CORE_API std::filesystem::path BuildGlobalShaderMapPath(const std::filesystem::path& cookedShaderRootPath);
	SPARKLE_CORE_API std::filesystem::path BuildCookedShaderLibraryPath(const std::filesystem::path& cookedShaderRootPath);

	// Runtime texture lookup accepts .stex products only and never searches source assets.
	SPARKLE_CORE_API std::optional<std::filesystem::path> ResolveCookedTexturePath(const std::filesystem::path& inputPath);
	SPARKLE_CORE_API void AppendCookedTexturePaths(std::span<const std::filesystem::path> inputPaths, std::vector<std::filesystem::path>& destination);
}

namespace Paths
{
	SPARKLE_CORE_API std::filesystem::path CookedSceneManifest(std::string_view sceneAssetId);
	SPARKLE_CORE_API std::filesystem::path CookedSceneManifestRelative(const std::filesystem::path& relativeManifestPath);
	SPARKLE_CORE_API std::filesystem::path CookedMeshAsset(std::uint64_t meshAssetId);
	SPARKLE_CORE_API std::filesystem::path CookedMaterialAsset(std::uint64_t materialAssetId);
	SPARKLE_CORE_API std::filesystem::path CookedSkeletonAsset(std::uint64_t skeletonAssetId);
	SPARKLE_CORE_API std::filesystem::path CookedAnimationAsset(std::uint64_t animationAssetId);
}
