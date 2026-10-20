#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace SparkleLauncher
{
	std::string MakeFreshnessSignaturePath(const std::filesystem::path& repositoryRoot, const std::filesystem::path& path);
	std::vector<std::filesystem::path> CollectBuildInputPaths(const std::filesystem::path& repositoryRoot);
	std::optional<std::string> ComputeSourceListHash(const std::filesystem::path& repositoryRoot);
}
