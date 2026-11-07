#include "PCH.h"

#include "Core/Public/Pixel/PixelFormat.h"

#include <cmath>

namespace Pixel
{
	std::uint8_t EncodeByteChannel(float linearValue, bool bApplySrgb) noexcept
	{
		const float clampedValue = (std::clamp) (linearValue, 0.0f, 1.0f);
		const float encodedValue = bApplySrgb ? (clampedValue <= 0.0031308f ? clampedValue * 12.92f : 1.055f * std::pow(clampedValue, 1.0f / 2.4f) - 0.055f) : clampedValue;
		return static_cast<std::uint8_t>((std::clamp) (std::lround(encodedValue * 255.0f), 0l, 255l));
	}

	float DecodeByteChannel(std::uint8_t byteValue, bool bApplySrgb) noexcept
	{
		const float normalizedValue = static_cast<float>(byteValue) / 255.0f;
		if (!bApplySrgb)
		{
			return normalizedValue;
		}

		return normalizedValue <= 0.04045f ? normalizedValue / 12.92f : std::pow((normalizedValue + 0.055f) / 1.055f, 2.4f);
	}
}
