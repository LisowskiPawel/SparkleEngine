#include "PCH.h"

#include "Core/Public/Identifiers/Uuid.h"

#include <array>
#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>

namespace Identifiers
{
	std::string CreateUuidV4String()
	{
		std::array<std::uint8_t, 16> bytes{};
		std::random_device random;
		for (std::uint8_t& byte : bytes)
		{
			byte = static_cast<std::uint8_t>(random());
		}
		bytes[6] = static_cast<std::uint8_t>((bytes[6] & 0x0fu) | 0x40u);
		bytes[8] = static_cast<std::uint8_t>((bytes[8] & 0x3fu) | 0x80u);

		std::ostringstream stream;
		stream << std::hex << std::setfill('0');
		for (std::size_t index = 0; index < bytes.size(); ++index)
		{
			if (index == 4u || index == 6u || index == 8u || index == 10u)
			{
				stream << '-';
			}
			stream << std::setw(2) << static_cast<unsigned int>(bytes[index]);
		}
		return stream.str();
	}

	bool IsUuidV4String(std::string_view value) noexcept
	{
		constexpr std::string_view variantDigits = "89ab";
		constexpr std::string_view hexadecimalDigits = "0123456789abcdef";
		if (value.size() != 36 || value[14] != '4')
		{
			return false;
		}
		const bool bValidVariant = variantDigits.find(value[19]) != std::string_view::npos;
		if (!bValidVariant)
		{
			return false;
		}

		for (std::size_t index = 0; index < value.size(); ++index)
		{
			const bool bFirstSeparatorPair = index == 8 || index == 13;
			const bool bSecondSeparatorPair = index == 18 || index == 23;
			const bool bSeparator = bFirstSeparatorPair || bSecondSeparatorPair;
			const char character = value[index];
			const bool bValidCharacter = bSeparator ? character == '-' : hexadecimalDigits.find(character) != std::string_view::npos;
			if (!bValidCharacter)
			{
				return false;
			}
		}
		return true;
	}

}
