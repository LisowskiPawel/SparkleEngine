#include "PCH.h"

#include "Core/Public/Strings/StringUtils.h"

namespace Strings
{
	static void AppendJsonControlEscape(unsigned char character, std::string& output)
	{
		constexpr std::string_view hexDigits = "0123456789abcdef";
		output += "\\u00";
		output.push_back(hexDigits[character >> 4u]);
		output.push_back(hexDigits[character & 0x0fu]);
	}

	std::string EscapeCsvField(std::string_view str)
	{
		constexpr std::size_t surroundingQuoteCount = 2;
		std::string result;
		result.reserve(str.size() + surroundingQuoteCount);
		result.push_back('"');
		for (const char character : str)
		{
			if (character == '"')
			{
				result.push_back('"');
			}
			result.push_back(character);
		}
		result.push_back('"');
		return result;
	}

	std::string EscapeJsonString(std::string_view str)
	{
		constexpr std::size_t escapeCapacitySlack = 8;
		std::string result;
		result.reserve(str.size() + escapeCapacitySlack);
		for (const char character : str)
		{
			switch (character)
			{
				case '\\':
					result += "\\\\";
					break;

				case '"':
					result += "\\\"";
					break;

				case '\n':
					result += "\\n";
					break;

				case '\r':
					result += "\\r";
					break;

				case '\t':
					result += "\\t";
					break;
				default:
					if (static_cast<unsigned char>(character) < 0x20u)
					{
						AppendJsonControlEscape(static_cast<unsigned char>(character), result);
					}
					else
					{
						result.push_back(character);
					}
					break;
			}
		}
		return result;
	}

}
