#include "PCH.h"

#include "Core/Public/Json/JsonReader.h"
#include "Json/JsonTokenReader.h"

namespace Json
{
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
			if (!TryReadPropertyName(objectText, cursor, property))
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

	bool TryReadObjectProperty(std::string_view objectText, std::string_view key, std::string_view& outValue)
	{
		outValue = {};
		std::size_t cursor = FindPropertyValue(objectText, key);
		if (cursor == std::string_view::npos || objectText[cursor] != '{')
		{
			return false;
		}
		const auto start = cursor;
		if (!SkipValue(objectText, cursor, 0))
		{
			return false;
		}
		outValue = objectText.substr(start, cursor - start);
		return true;
	}

	bool TryReadArrayProperty(std::string_view objectText, std::string_view key, std::vector<std::string_view>& outElements)
	{
		outElements.clear();
		std::size_t cursor = FindPropertyValue(objectText, key);
		if (cursor == std::string_view::npos || objectText[cursor++] != '[')
		{
			return false;
		}
		SkipWhitespace(objectText, cursor);
		while (cursor < objectText.size() && objectText[cursor] != ']')
		{
			const std::size_t start = cursor;
			if (!SkipValue(objectText, cursor, 0u))
			{
				outElements.clear();
				return false;
			}
			outElements.push_back(objectText.substr(start, cursor - start));
			SkipWhitespace(objectText, cursor);
			if (objectText[cursor] == ']')
			{
				break;
			}
			++cursor;
			SkipWhitespace(objectText, cursor);
		}
		return true;
	}

}
