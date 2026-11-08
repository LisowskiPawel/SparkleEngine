#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>

namespace Pixel
{
	// sRGB <-> Linear conversion for byte channels
	// Converts between normalized float [0.0, 1.0] and sRGB-encoded byte [0, 255]
	SPARKLE_CORE_API std::uint8_t EncodeByteChannel(float linearValue, bool bApplySrgb) noexcept;
	SPARKLE_CORE_API float DecodeByteChannel(std::uint8_t byteValue, bool bApplySrgb) noexcept;
}
