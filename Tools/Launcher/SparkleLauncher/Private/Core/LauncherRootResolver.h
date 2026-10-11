#pragma once

#include "SparkleLauncher/RepositoryLocator.h"

#include <filesystem>
#include <optional>
#include <string>

namespace SparkleLauncher
{
	std::optional<std::filesystem::path> TryResolveLauncherRoot(const std::filesystem::path& requestedRoot, const std::filesystem::path& launcherDirectory, std::string& outErrorMessage);
}
