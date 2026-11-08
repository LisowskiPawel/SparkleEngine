#include "PCH.h"

#include "Json/JsonTokenReader.h"

#include "Core/Public/Strings/StringUtils.h"

#include <cctype>
#include <utility>

namespace Json
{
	static bool TryReadHexQuad(std::string_view document, std::size_t& cursor, std::uint32_t& codeUnit) noexcept
	{
		if (cursor + 4u > document.size())
		{
			return false;
		}

		codeUnit = 0u;
		for (std::size_t index = 0; index < 4u; ++index)
		{
			const char hex = document[cursor++];
			codeUnit <<= 4u;
			if (hex >= '0' && hex <= '9')
			{
				codeUnit |= static_cast<std::uint32_t>(hex - '0');
			}
			else if (hex >= 'a' && hex <= 'f')
			{
				codeUnit |= static_cast<std::uint32_t>(hex - 'a' + 10);
			}
			else if (hex >= 'A' && hex <= 'F')
			{
				codeUnit |= static_cast<std::uint32_t>(hex - 'A' + 10);
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	static bool TryReadUnicodeEscape(std::string_view document, std::size_t& cursor, std::string& value)
	{
		std::uint32_t codePoint = 0u;
		if (!TryReadHexQuad(document, cursor, codePoint))
		{
			return false;
		}

		if (codePoint >= 0xd800u && codePoint <= 0xdbffu)
		{
			if (cursor + 2u > document.size() || document[cursor] != '\\' || document[cursor + 1u] != 'u')
			{
				return false;
			}

			cursor += 2u;
			std::uint32_t lowSurrogate = 0u;
			if (!TryReadHexQuad(document, cursor, lowSurrogate) || lowSurrogate < 0xdc00u || lowSurrogate > 0xdfffu)
			{
				return false;
			}
			codePoint = 0x10000u + ((codePoint - 0xd800u) << 10u) + (lowSurrogate - 0xdc00u);
		}
		else if (codePoint >= 0xdc00u && codePoint <= 0xdfffu)
		{
			return false;
		}

		return Strings::AppendUtf8CodePoint(codePoint, value);
	}

	static bool TryReadStringEscape(std::string_view document, std::size_t& cursor, std::string& value)
	{
		if (cursor >= document.size())
		{
			return false;
		}
		const char escape = document[cursor++];
		if (escape == 'u')
		{
			return TryReadUnicodeEscape(document, cursor, value);
		}
		constexpr std::string_view escapeCodes = "\"\\/bfnrt";
		constexpr std::string_view escapedCharacters = "\"\\/\b\f\n\r\t";
		static_assert(escapeCodes.size() == escapedCharacters.size());
		const std::size_t index = escapeCodes.find(escape);
		if (index == std::string_view::npos)
		{
			return false;
		}
		value.push_back(escapedCharacters[index]);
		return true;
	}

	bool TryReadString(std::string_view document, std::size_t& cursor, std::string& outValue)
	{
		if (cursor >= document.size() || document[cursor++] != '"')
		{
			return false;
		}

		std::string value;
		while (cursor < document.size())
		{
			const unsigned char character = static_cast<unsigned char>(document[cursor++]);
			if (character == '"')
			{
				if (!Strings::IsValidUtf8(value))
				{
					return false;
				}
				outValue = std::move(value);
				return true;
			}
			if (character < 0x20u)
			{
				return false;
			}
			if (character != '\\')
			{
				value.push_back(static_cast<char>(character));
				continue;
			}
			if (!TryReadStringEscape(document, cursor, value))
			{
				return false;
			}
		}
		return false;
	}

	void SkipWhitespace(std::string_view document, std::size_t& cursor) noexcept
	{
		while (cursor < document.size() && (document[cursor] == ' ' || document[cursor] == '\t' || document[cursor] == '\r' || document[cursor] == '\n'))
		{
			++cursor;
		}
	}

	static bool SkipNumber(std::string_view document, std::size_t& cursor)
	{
		if (document[cursor] == '-')
		{
			++cursor;
		}
		if (cursor == document.size() || document[cursor] < '0' || document[cursor] > '9')
		{
			return false;
		}
		if (document[cursor++] != '0')
		{
			while (cursor < document.size() && std::isdigit(static_cast<unsigned char>(document[cursor])))
			{
				++cursor;
			}
		}

		if (cursor < document.size() && document[cursor] == '.')
		{
			const std::size_t fraction = ++cursor;
			while (cursor < document.size() && std::isdigit(static_cast<unsigned char>(document[cursor])))
			{
				++cursor;
			}
			if (cursor == fraction)
			{
				return false;
			}
		}
		if (cursor < document.size() && (document[cursor] == 'e' || document[cursor] == 'E'))
		{
			++cursor;
			if (cursor < document.size() && (document[cursor] == '+' || document[cursor] == '-'))
			{
				++cursor;
			}
			const std::size_t exponent = cursor;
			while (cursor < document.size() && std::isdigit(static_cast<unsigned char>(document[cursor])))
			{
				++cursor;
			}
			if (cursor == exponent)
			{
				return false;
			}
		}
		return true;
	}

	bool TryReadPropertyName(std::string_view document, std::size_t& cursor, std::string& name)
	{
		if (!TryReadString(document, cursor, name))
		{
			return false;
		}
		SkipWhitespace(document, cursor);
		return cursor < document.size() && document[cursor++] == ':';
	}

	static bool SkipContainer(std::string_view document, std::size_t& cursor, std::size_t depth)
	{
		const bool bIsObject = document[cursor++] == '{';
		const char end = bIsObject ? '}' : ']';
		SkipWhitespace(document, cursor);
		if (cursor < document.size() && document[cursor] == end)
		{
			++cursor;
			return true;
		}
		while (cursor < document.size())
		{
			if (bIsObject)
			{
				std::string ignored;
				if (!TryReadPropertyName(document, cursor, ignored))
				{
					return false;
				}
			}
			if (!SkipValue(document, cursor, depth + 1u))
			{
				return false;
			}
			SkipWhitespace(document, cursor);
			if (cursor == document.size())
			{
				return false;
			}
			if (document[cursor] == end)
			{
				++cursor;
				return true;
			}
			if (document[cursor++] != ',')
			{
				return false;
			}
			SkipWhitespace(document, cursor);
		}
		return false;
	}

	bool SkipValue(std::string_view document, std::size_t& cursor, std::size_t depth)
	{
		SkipWhitespace(document, cursor);
		if (cursor == document.size() || depth > 64u)
		{
			return false;
		}
		if (document[cursor] == '"')
		{
			std::string ignored;
			return TryReadString(document, cursor, ignored);
		}
		if (document[cursor] == '{' || document[cursor] == '[')
		{
			return SkipContainer(document, cursor, depth);
		}
		for (const std::string_view literal : {"true", "false", "null"})
		{
			if (document.substr(cursor).starts_with(literal))
			{
				cursor += literal.size();
				return true;
			}
		}
		return SkipNumber(document, cursor);
	}

}
