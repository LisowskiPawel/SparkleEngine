#pragma once

#include "Core/Public/CoreAPI.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace Paths
{
	SPARKLE_CORE_API std::string_view GetFileName(std::string_view path) noexcept;
	SPARKLE_CORE_API std::string MakeProjectRelativeString(const std::filesystem::path& path);
	SPARKLE_CORE_API std::filesystem::path Normalize(const std::filesystem::path& path);
	SPARKLE_CORE_API std::optional<std::filesystem::path> ResolveRelativePath(const std::filesystem::path& baseDirectory, const std::filesystem::path& path);
	SPARKLE_CORE_API bool IsUnderRoot(const std::filesystem::path& path, const std::filesystem::path& root);
	SPARKLE_CORE_API std::optional<std::filesystem::path> TryMakeRelativeUnderRoot(const std::filesystem::path& path, const std::filesystem::path& root);
	SPARKLE_CORE_API std::string MakeSafePathComponent(std::string_view value);
	SPARKLE_CORE_API std::wstring MakePathKey(const std::filesystem::path& path);
	SPARKLE_CORE_API std::wstring GetLowercaseExtension(const std::filesystem::path& path);
}
