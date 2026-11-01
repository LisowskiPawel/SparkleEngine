#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Filesystem
{
	inline constexpr std::string_view kWorkspaceMarker = ".sparkle";
	inline constexpr std::string_view kEngineMarker = ".sparkle-engine";
	inline constexpr std::string_view kProjectMarker = ".sparkle-project";

	// Process paths
	const std::filesystem::path& GetWorkingDirectory();
	std::filesystem::path GetExecutablePath();
	std::filesystem::path GetExecutableDirectory();

	// Stateless root discovery for early bootstrap code
	std::filesystem::path ResolveWorkspaceRootPath();
	std::filesystem::path ResolveArtifactRootPath(const std::filesystem::path& workspaceRoot);

	// Cached workspace and content roots
	const std::filesystem::path& GetWorkspaceRootPath();
	const std::filesystem::path& GetProjectPath();
	const std::filesystem::path& GetEnginePath();
	void ConfigureProjectRoot(const std::filesystem::path& projectRoot);

	// Cooked product paths
	const std::filesystem::path& GetCookedAssetRootPath();
	const std::filesystem::path& GetCookedShaderRootPath();
	const std::filesystem::path& GetGlobalShaderMapPath();
	const std::filesystem::path& GetCookedShaderLibraryPath();
	const std::filesystem::path& GetCookedTextureRootPath();
	const std::filesystem::path& GetCookedSceneManifestRootPath();
	const std::filesystem::path& GetCookedMeshRootPath();
	const std::filesystem::path& GetCookedMaterialRootPath();
	const std::filesystem::path& GetCookedSkeletonRootPath();
	const std::filesystem::path& GetCookedAnimationRootPath();
	const std::filesystem::path& GetSceneAssetRegistryPath();
	std::filesystem::path BuildGlobalShaderMapPath(const std::filesystem::path& cookedShaderRootPath);
	std::filesystem::path BuildCookedShaderLibraryPath(const std::filesystem::path& cookedShaderRootPath);

	// Marker-based workspace and content discovery
	std::optional<std::filesystem::path> FindAncestorWithMarker(const std::filesystem::path& startDir, std::string_view markerFileName, uint32_t maxDepth = 32);

	std::optional<std::filesystem::path> DiscoverWorkspaceRoot();

	std::optional<std::filesystem::path> DiscoverEngineRoot();

	std::optional<std::filesystem::path> DiscoverProjectRoot();

	// Runtime texture lookup accepts .stex products only and never searches source assets.
	std::optional<std::filesystem::path> ResolveCookedTexturePath(const std::filesystem::path& inputPath);
	void AppendCookedTexturePaths(std::span<const std::filesystem::path> inputPaths, std::vector<std::filesystem::path>& destination);
}
