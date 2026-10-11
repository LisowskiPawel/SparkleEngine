#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace SparkleLauncher
{
	bool IsRepositoryRoot(const std::filesystem::path& path);
	std::optional<std::filesystem::path> TryOpenRepositoryRoot(const std::filesystem::path& rootPath, std::string& outErrorMessage);
	std::optional<std::filesystem::path> TryFindRepositoryRoot(const std::filesystem::path& startPath, std::string& outErrorMessage);
}
