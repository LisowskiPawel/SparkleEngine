#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>

namespace Pixel
{
	// IEEE 754 float32 to float16 (half precision) conversion
	// Finite mantissas truncate; signed zero, subnormals, infinity and NaN retain their categories.
	SPARKLE_CORE_API std::uint16_t FloatToHalf(float value) noexcept;
	SPARKLE_CORE_API float HalfToFloat(std::uint16_t value) noexcept;
}
