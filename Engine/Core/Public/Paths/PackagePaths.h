#pragma once

#include "Core/Public/CoreAPI.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace Filesystem
{
	SPARKLE_CORE_API std::optional<std::filesystem::path> DiscoverPackageRoot(const std::filesystem::path& startPath = {});
	SPARKLE_CORE_API bool TryReadPackageManifest(const std::filesystem::path& packageRoot, std::string& outDocument);
	SPARKLE_CORE_API std::optional<std::filesystem::path> DiscoverPackageProjectRoot(const std::filesystem::path& packageRoot);
	SPARKLE_CORE_API std::optional<std::filesystem::path> DiscoverPackageEngineRoot(const std::filesystem::path& packageRoot);
	SPARKLE_CORE_API std::optional<std::filesystem::path> ResolvePackageContentRoot(const std::filesystem::path& packageRoot);
	SPARKLE_CORE_API std::optional<std::filesystem::path> ResolvePackageSourceRoot(const std::filesystem::path& packageRoot);
	// A caller processing a manifest can reuse its document; no manifest state is retained.
	SPARKLE_CORE_API std::optional<std::filesystem::path> ResolvePackageFilePath(const std::filesystem::path& packageRoot, std::string_view relativePath, std::string_view manifest = {});
	SPARKLE_CORE_API std::optional<std::filesystem::path> ResolvePackageProductPath(const std::filesystem::path& packageRoot, std::string_view target, std::string_view configuration);
}
