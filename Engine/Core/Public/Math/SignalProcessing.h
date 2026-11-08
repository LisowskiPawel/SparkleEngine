#pragma once

#include "Core/Public/CoreAPI.h"

namespace MathUtils
{
	// Modified Bessel function of the first kind, order 0
	// Used in Kaiser window calculations for windowed-sinc filtering
	SPARKLE_CORE_API float BesselI0(float x) noexcept;

	// Sinc function: sin(πx) / (πx), with special handling for x near 0
	SPARKLE_CORE_API float Sinc(float x) noexcept;

}
