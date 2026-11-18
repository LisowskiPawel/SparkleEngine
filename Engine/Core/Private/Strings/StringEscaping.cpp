#include "PCH.h"

#include "Core/Public/Strings/StringUtils.h"

namespace Strings
{
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

}
