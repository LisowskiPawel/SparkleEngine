#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace SparkleLauncher
{
	bool IsWorkspaceRoot(const std::filesystem::path& path);
	std::optional<std::filesystem::path> TryResolveWorkspaceRoot(const std::filesystem::path& rootPath, std::string& outErrorMessage);
	std::optional<std::filesystem::path> TryFindWorkspaceRoot(const std::filesystem::path& startPath, std::string& outErrorMessage);
}
