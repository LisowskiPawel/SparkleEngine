#include "PCH.h"

#include "Core/Public/Strings/StringUtils.h"

#include <cstdint>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Strings
{
	bool IsValidUtf8(std::string_view text) noexcept
	{
		for (std::size_t index = 0; index < text.size();)
		{
			const std::uint8_t lead = static_cast<std::uint8_t>(text[index++]);
			if (lead <= 0x7fu)
			{
				continue;
			}

			std::uint32_t codePoint = 0u;
			std::size_t continuationCount = 0u;
			std::uint32_t minimumCodePoint = 0u;
			if ((lead & 0xe0u) == 0xc0u)
			{
				codePoint = lead & 0x1fu;
				continuationCount = 1u;
				minimumCodePoint = 0x80u;
			}
			else if ((lead & 0xf0u) == 0xe0u)
			{
				codePoint = lead & 0x0fu;
				continuationCount = 2u;
				minimumCodePoint = 0x800u;
			}
			else if ((lead & 0xf8u) == 0xf0u)
			{
				codePoint = lead & 0x07u;
				continuationCount = 3u;
				minimumCodePoint = 0x10000u;
			}
			else
			{
				return false;
			}

			if (index + continuationCount > text.size())
			{
				return false;
			}
			for (std::size_t continuation = 0; continuation < continuationCount; ++continuation)
			{
				const std::uint8_t value = static_cast<std::uint8_t>(text[index++]);
				if ((value & 0xc0u) != 0x80u)
				{
					return false;
				}
				codePoint = (codePoint << 6u) | (value & 0x3fu);
			}

			if (codePoint < minimumCodePoint || codePoint > 0x10ffffu || (codePoint >= 0xd800u && codePoint <= 0xdfffu))
			{
				return false;
			}
		}
		return true;
	}

	bool AppendUtf8CodePoint(std::uint32_t codePoint, std::string& output)
	{
		if (codePoint <= 0x7fu)
		{
			output.push_back(static_cast<char>(codePoint));
		}
		else if (codePoint <= 0x7ffu)
		{
			output.push_back(static_cast<char>(0xc0u | (codePoint >> 6u)));
			output.push_back(static_cast<char>(0x80u | (codePoint & 0x3fu)));
		}
		else if (codePoint <= 0xffffu && (codePoint < 0xd800u || codePoint > 0xdfffu))
		{
			output.push_back(static_cast<char>(0xe0u | (codePoint >> 12u)));
			output.push_back(static_cast<char>(0x80u | ((codePoint >> 6u) & 0x3fu)));
			output.push_back(static_cast<char>(0x80u | (codePoint & 0x3fu)));
		}
		else if (codePoint >= 0x10000u && codePoint <= 0x10ffffu)
		{
			output.push_back(static_cast<char>(0xf0u | (codePoint >> 18u)));
			output.push_back(static_cast<char>(0x80u | ((codePoint >> 12u) & 0x3fu)));
			output.push_back(static_cast<char>(0x80u | ((codePoint >> 6u) & 0x3fu)));
			output.push_back(static_cast<char>(0x80u | (codePoint & 0x3fu)));
		}
		else
		{
			return false;
		}
		return true;
	}

	std::wstring ToWide(std::string_view str)
	{
		if (str.empty())
		{
			return {};
		}
#if defined(_WIN32)
		if (str.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
		{
			throw std::length_error("UTF-8 text is too long.");
		}
		const int size = static_cast<int>(str.size());
		const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), size, nullptr, 0);
		if (length == 0)
		{
			throw std::invalid_argument("Invalid UTF-8 text.");
		}
		std::wstring result(static_cast<std::size_t>(length), L'\0');
		MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), size, result.data(), length);
		return result;
#else
		if (!IsValidUtf8(str))
		{
			throw std::invalid_argument("Invalid UTF-8 text.");
		}
		std::wstring result;
		for (std::size_t cursor = 0; cursor < str.size();)
		{
			const auto lead = static_cast<std::uint8_t>(str[cursor++]);
			const unsigned count = lead <= 0x7fu ? 0u : lead <= 0xdfu ? 1u : lead <= 0xefu ? 2u : 3u;
			std::uint32_t codePoint = lead & (count == 0u ? 0x7fu : 0x7fu >> count);
			for (unsigned index = 0; index < count; ++index)
			{
				codePoint = (codePoint << 6u) | (static_cast<std::uint8_t>(str[cursor++]) & 0x3fu);
			}
			result.push_back(static_cast<wchar_t>(codePoint));
		}
		return result;
#endif
	}

	std::wstring ToWide(const std::filesystem::path& path)
	{
		return path.wstring();
	}

	std::string ToNarrow(std::wstring_view str)
	{
		if (str.empty())
		{
			return {};
		}
#if defined(_WIN32)
		if (str.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
		{
			throw std::length_error("Wide text is too long.");
		}
		const int size = static_cast<int>(str.size());
		const int length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.data(), size, nullptr, 0, nullptr, nullptr);
		if (length == 0)
		{
			throw std::invalid_argument("Invalid wide text.");
		}
		std::string result(static_cast<std::size_t>(length), '\0');
		WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.data(), size, result.data(), length, nullptr, nullptr);
		return result;
#else
		std::string result;
		for (const wchar_t character : str)
		{
			if (!AppendUtf8CodePoint(static_cast<std::uint32_t>(character), result))
			{
				throw std::invalid_argument("Invalid wide text.");
			}
		}
		return result;
#endif
	}
}
