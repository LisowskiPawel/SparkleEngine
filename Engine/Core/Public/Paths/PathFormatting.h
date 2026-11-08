#pragma once

#include "Core/Public/CoreAPI.h"

#include <filesystem>
#include <string>
#include <string_view>

namespace PathFormatting
{
	SPARKLE_CORE_API std::string TimestampForFileName();
	SPARKLE_CORE_API std::filesystem::path TimestampedFileName(std::string_view stem, std::string_view extension);
	SPARKLE_CORE_API bool IsAsciiAlphaNumeric(char character) noexcept;
	SPARKLE_CORE_API char ToLowerAscii(char character) noexcept;
	SPARKLE_CORE_API std::string SanitizePathSegment(std::string_view segment, std::string_view fallback = "Unknown");
	SPARKLE_CORE_API bool EndsWithIgnoreCase(std::string_view value, std::string_view suffix) noexcept;
}
