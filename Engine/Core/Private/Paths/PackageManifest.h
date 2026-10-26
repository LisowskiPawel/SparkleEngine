#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

namespace Filesystem::Private
{
	bool HasReparsePath(const std::filesystem::path& root, const std::filesystem::path& relative);
	std::optional<std::filesystem::path> ReadPackageRelativePath(const std::filesystem::path& packageRoot, std::string_view property);
	std::optional<std::filesystem::path> ResolveContainedPackageDirectory(const std::filesystem::path& packageRoot, std::string_view property);
}
