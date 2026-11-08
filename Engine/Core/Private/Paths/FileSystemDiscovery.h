#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace Filesystem
{
	std::optional<std::filesystem::path> FindProcessAncestorWithMarker(std::string_view marker);
	std::optional<std::filesystem::path> DiscoverWorkspaceProjectRoot();
	std::string GetExecutableStem();
	std::string InferProjectNameFromExecutableStem(std::string executableStem);
}
