#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace Filesystem
{
	inline constexpr std::string_view kWorkspaceMarker = ".sparkle";
	inline constexpr std::string_view kEngineMarker = ".sparkle-engine";
	inline constexpr std::string_view kProjectMarker = ".sparkle-project";

	// Process paths
	SPARKLE_CORE_API std::filesystem::path GetExecutablePath();
	SPARKLE_CORE_API std::filesystem::path GetExecutableDirectory();

	// Stateless root discovery for early bootstrap code
	SPARKLE_CORE_API std::filesystem::path ResolveWorkspaceRootPath();
	SPARKLE_CORE_API std::filesystem::path ResolveArtifactRootPath(const std::filesystem::path& workspaceRoot);

	// Cached workspace and content roots
	SPARKLE_CORE_API const std::filesystem::path& GetWorkspaceRootPath();
	SPARKLE_CORE_API const std::filesystem::path& GetProjectPath();
	SPARKLE_CORE_API const std::filesystem::path& GetEnginePath();
	SPARKLE_CORE_API void ConfigureProjectRoot(const std::filesystem::path& projectRoot);

	// Marker-based workspace and content discovery
	SPARKLE_CORE_API std::optional<std::filesystem::path> FindAncestorWithMarker(const std::filesystem::path& startDir, std::string_view markerFileName, uint32_t maxDepth = 32);

	SPARKLE_CORE_API std::optional<std::filesystem::path> DiscoverWorkspaceRoot();

	SPARKLE_CORE_API std::optional<std::filesystem::path> DiscoverEngineRoot();

	SPARKLE_CORE_API std::optional<std::filesystem::path> DiscoverProjectRoot();
}
