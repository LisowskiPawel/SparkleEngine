#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>

enum class ButtonState : std::uint8_t
{
	Up = 0,
	Pressed = 1,
	Held = 2,
	Released = 3,
};

SPARKLE_CORE_API bool IsDown(ButtonState state) noexcept;
SPARKLE_CORE_API bool IsUp(ButtonState state) noexcept;
SPARKLE_CORE_API bool IsEdge(ButtonState state) noexcept;
SPARKLE_CORE_API bool IsContinuous(ButtonState state) noexcept;
