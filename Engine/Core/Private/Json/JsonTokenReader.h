#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace Json
{
	bool TryReadString(std::string_view document, std::size_t& cursor, std::string& outValue);
	bool TryReadPropertyName(std::string_view document, std::size_t& cursor, std::string& name);
	void SkipWhitespace(std::string_view document, std::size_t& cursor) noexcept;
	bool SkipValue(std::string_view document, std::size_t& cursor, std::size_t depth);
}
