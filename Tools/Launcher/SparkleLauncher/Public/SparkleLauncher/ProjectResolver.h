#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace SparkleLauncher
{
	struct LauncherProject
	{
		std::string Id;
		std::filesystem::path RootPath;
	};

	std::optional<LauncherProject> ResolveLauncherProject(const std::filesystem::path& repositoryRoot, std::string& outErrorMessage);
}
