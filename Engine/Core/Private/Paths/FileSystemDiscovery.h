#pragma once

#include "Core/Public/Paths/PackagePaths.h"

#include <filesystem>
#include <optional>
#include <string>

namespace Filesystem::Private
{
	std::string GetExecutableStem();
	std::string InferProjectNameFromExecutableStem(std::string executableStem);
	std::optional<std::filesystem::path> DiscoverWorkspaceProjectRoot();
}
