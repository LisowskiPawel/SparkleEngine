#include "PCH.h"

#include "Core/Public/Json/JsonReader.h"

#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Strings/StringUtils.h"

#include <cctype>
#include <utility>

namespace Json
{
	static bool TryReadString(std::string_view document, std::size_t& cursor, std::string& outValue)
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
			if (cursor >= document.size())
			{
				return false;
			}

			const char escape = document[cursor++];
			switch (escape)
			{
				case '"':
					value.push_back('"');
					break;
				case '\\':
					value.push_back('\\');
					break;
				case '/':
					value.push_back('/');
					break;
				case 'b':
					value.push_back('\b');
					break;
				case 'f':
					value.push_back('\f');
					break;
				case 'n':
					value.push_back('\n');
					break;
				case 'r':
					value.push_back('\r');
					break;
				case 't':
					value.push_back('\t');
					break;
				case 'u':
				{
					auto readHexQuad = [&document, &cursor](std::uint32_t& codeUnit) noexcept
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
					};

					std::uint32_t codePoint = 0u;
					if (!readHexQuad(codePoint))
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
						if (!readHexQuad(lowSurrogate) || lowSurrogate < 0xdc00u || lowSurrogate > 0xdfffu)
						{
							return false;
						}
						codePoint = 0x10000u + ((codePoint - 0xd800u) << 10u) + (lowSurrogate - 0xdc00u);
					}
					else if (codePoint >= 0xdc00u && codePoint <= 0xdfffu)
					{
						return false;
					}
					if (!Strings::AppendUtf8CodePoint(codePoint, value))
					{
						return false;
					}
					break;
				}
				default:
					return false;
			}
		}
		return false;
	}

	static void SkipWhitespace(std::string_view document, std::size_t& cursor) noexcept
	{
		while (cursor < document.size() && (document[cursor] == ' ' || document[cursor] == '\t' || document[cursor] == '\r' || document[cursor] == '\n'))
		{
			++cursor;
		}
	}

	static bool SkipValue(std::string_view document, std::size_t& cursor, std::size_t depth)
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
			const bool isObject = document[cursor++] == '{';
			const char end = isObject ? '}' : ']';
			SkipWhitespace(document, cursor);
			if (cursor < document.size() && document[cursor] == end)
			{
				++cursor;
				return true;
			}
			while (cursor < document.size())
			{
				if (isObject)
				{
					std::string ignored;
					if (!TryReadString(document, cursor, ignored))
					{
						return false;
					}
					SkipWhitespace(document, cursor);
					if (cursor == document.size() || document[cursor++] != ':')
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
		for (const std::string_view literal : {"true", "false", "null"})
		{
			if (document.substr(cursor).starts_with(literal))
			{
				cursor += literal.size();
				return true;
			}
		}
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

	std::size_t FindPropertyValue(std::string_view objectText, std::string_view key)
	{
		std::size_t cursor = objectText.starts_with("\xef\xbb\xbf") ? 3u : 0u;
		SkipWhitespace(objectText, cursor);
		if (cursor == objectText.size() || objectText[cursor++] != '{')
		{
			return std::string_view::npos;
		}
		std::size_t result = std::string_view::npos;
		SkipWhitespace(objectText, cursor);
		while (cursor < objectText.size() && objectText[cursor] != '}')
		{
			std::string property;
			if (!TryReadString(objectText, cursor, property))
			{
				return std::string_view::npos;
			}
			SkipWhitespace(objectText, cursor);
			if (cursor == objectText.size() || objectText[cursor++] != ':')
			{
				return std::string_view::npos;
			}
			SkipWhitespace(objectText, cursor);
			if (property == key)
			{
				if (result != std::string_view::npos)
				{
					return std::string_view::npos;
				}
				result = cursor;
			}
			if (!SkipValue(objectText, cursor, 0u))
			{
				return std::string_view::npos;
			}
			SkipWhitespace(objectText, cursor);
			if (cursor < objectText.size() && objectText[cursor] == '}')
			{
				break;
			}
			if (cursor == objectText.size() || objectText[cursor++] != ',')
			{
				return std::string_view::npos;
			}
			SkipWhitespace(objectText, cursor);
			if (cursor == objectText.size() || objectText[cursor] == '}')
			{
				return std::string_view::npos;
			}
		}
		if (cursor == objectText.size() || objectText[cursor++] != '}')
		{
			return std::string_view::npos;
		}
		SkipWhitespace(objectText, cursor);
		return cursor == objectText.size() ? result : std::string_view::npos;
	}

	bool TryReadStringProperty(std::string_view objectText, std::string_view key, std::string& outValue)
	{
		std::size_t cursor = FindPropertyValue(objectText, key);
		return cursor != std::string_view::npos && TryReadString(objectText, cursor, outValue);
	}

	bool TryReadUInt64Property(std::string_view objectText, std::string_view key, std::uint64_t& outValue)
	{
		return TryReadUnsignedIntegerProperty(objectText, key, outValue);
	}

	bool TryParseHexUInt64(std::string_view text, std::uint64_t& outValue) noexcept
	{
		return Formatting::TryParseHexUInt64(text, outValue);
	}
}
