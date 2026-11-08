#include "PCH.h"

#include "Core/Public/Strings/StringUtils.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Strings
{
	bool IsAsciiWhitespace(char character) noexcept
	{
		return character == ' ' || character == '\t' || character == '\r' || character == '\n';
	}

	bool ContainsAsciiWhitespace(std::string_view str) noexcept
	{
		return std::ranges::any_of(str, IsAsciiWhitespace);
	}

	bool IsNullTerminated(std::span<const char> buffer) noexcept
	{
		return std::ranges::any_of(buffer, [](const char character) { return character == '\0'; });
	}

	std::string_view TrimAsciiWhitespace(std::string_view str) noexcept
	{
		constexpr std::string_view whitespace = " \t\r\n";
		const std::size_t start = str.find_first_not_of(whitespace);
		if (start == std::string_view::npos)
		{
			return {};
		}
		const std::size_t end = str.find_last_not_of(whitespace);
		return str.substr(start, end - start + 1);
	}

	std::string_view Unquote(std::string_view str) noexcept
	{
		constexpr std::size_t quoteCharacterCount = 2;
		if (str.size() >= quoteCharacterCount && str.front() == '"' && str.back() == '"')
		{
			return str.substr(1, str.size() - quoteCharacterCount);
		}
		return str;
	}

	std::string TrimCopy(std::string_view str)
	{
		return std::string(TrimAsciiWhitespace(str));
	}

	std::string UnquoteCopy(std::string_view str)
	{
		return std::string(Unquote(TrimAsciiWhitespace(str)));
	}

	std::vector<std::string_view> Split(std::string_view str, char separator, bool bKeepEmptyFields)
	{
		std::vector<std::string_view> fields;
		std::size_t start = 0;
		while (start <= str.size())
		{
			const std::size_t separatorIndex = str.find(separator, start);
			const std::size_t end = separatorIndex == std::string_view::npos ? str.size() : separatorIndex;
			const std::string_view field = str.substr(start, end - start);
			if (bKeepEmptyFields || !field.empty())
			{
				fields.push_back(field);
			}

			if (separatorIndex == std::string_view::npos)
			{
				break;
			}
			start = separatorIndex + 1;
		}
		return fields;
	}

	std::string ToLowerCopy(std::string_view str)
	{
		std::string lowered(str);
		std::ranges::transform(lowered, lowered.begin(), [](const unsigned char character) { return static_cast<char>(std::tolower(character)); });
		return lowered;
	}

	bool EqualsIgnoreCase(std::string_view lhs, std::string_view rhs) noexcept
	{
		if (lhs.size() != rhs.size())
		{
			return false;
		}

		for (std::size_t index = 0; index < lhs.size(); ++index)
		{
			const unsigned char lhsChar = static_cast<unsigned char>(lhs[index]);
			const unsigned char rhsChar = static_cast<unsigned char>(rhs[index]);
			if (std::tolower(lhsChar) != std::tolower(rhsChar))
			{
				return false;
			}
		}

		return true;
	}

	bool StartsWithIgnoreCase(std::string_view value, std::string_view prefix) noexcept
	{
		return prefix.size() <= value.size() && EqualsIgnoreCase(value.substr(0, prefix.size()), prefix);
	}

	bool ContainsIgnoreCase(std::string_view haystack, std::string_view needle) noexcept
	{
		if (needle.empty())
		{
			return true;
		}

		if (needle.size() > haystack.size())
		{
			return false;
		}

		const std::size_t finalStartIndex = haystack.size() - needle.size();
		for (std::size_t startIndex = 0; startIndex <= finalStartIndex; ++startIndex)
		{
			if (EqualsIgnoreCase(haystack.substr(startIndex, needle.size()), needle))
			{
				return true;
			}
		}

		return false;
	}

	bool TryParseBool(std::string_view str, bool& bOutValue)
	{
		const std::string_view trimmed = TrimAsciiWhitespace(str);
		const bool bIsTrueTokenTrimmed = EqualsIgnoreCase(trimmed, "true") || trimmed == "1";
		const bool bIsTrueTokenIgnoreCase = bIsTrueTokenTrimmed || (EqualsIgnoreCase(trimmed, "yes"));
		const bool bIsTrueToken = bIsTrueTokenIgnoreCase || (EqualsIgnoreCase(trimmed, "on"));

		if (bIsTrueToken)
		{
			bOutValue = true;
			return true;
		}

		const bool bIsFalseTokenTrimmed = EqualsIgnoreCase(trimmed, "false") || trimmed == "0";
		const bool bIsFalseTokenIgnoreCase = bIsFalseTokenTrimmed || (EqualsIgnoreCase(trimmed, "no"));
		const bool bIsFalseToken = bIsFalseTokenIgnoreCase || (EqualsIgnoreCase(trimmed, "off"));

		if (bIsFalseToken)
		{
			bOutValue = false;
			return true;
		}

		return false;
	}

	bool TrySplitKeyValue(std::string_view str, char separator, std::string_view& outKey, std::string_view& outValue) noexcept
	{
		const std::size_t separatorIndex = str.find(separator);
		if (separatorIndex == std::string::npos)
		{
			return false;
		}

		outKey = TrimAsciiWhitespace(str.substr(0, separatorIndex));
		outValue = TrimAsciiWhitespace(str.substr(separatorIndex + 1));
		return true;
	}

	std::string Join(std::span<const std::string_view> values, std::string_view separator, std::size_t firstValueIndex)
	{
		std::string output;
		for (std::size_t index = firstValueIndex; index < values.size(); ++index)
		{
			if (index > firstValueIndex)
			{
				output += separator;
			}
			output += values[index];
		}
		return output;
	}

}
