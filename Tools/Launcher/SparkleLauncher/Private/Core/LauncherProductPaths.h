#pragma once
#include <filesystem>
#include <string_view>

namespace SparkleLauncher
{
	bool HasBuildSources(const std::filesystem::path& root);
	std::filesystem::path ResolveToolExecutable(const std::filesystem::path& root, std::string_view target, std::string_view profile);
	std::filesystem::path ResolveProjectExecutable(const std::filesystem::path& root, std::string_view project, std::string_view role, std::string_view profile, std::string_view variant = {});
	std::filesystem::path ResolveProjectRoot(const std::filesystem::path& root, std::string_view project);
	std::filesystem::path ResolveCookedRoot(const std::filesystem::path& root, std::string_view project);
}
