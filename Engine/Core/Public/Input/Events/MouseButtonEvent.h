#pragma once

#include "../../CoreAPI.h"
#include "../Mouse/MouseButton.h"
#include "../Mouse/MousePosition.h"
#include "../Keyboard/ModifierFlags.h"
#include "../Device/InputDevice.h"

struct MouseButtonEvent
{
	MouseButton Button = MouseButton::Left;
	MousePosition Position;
	ModifierFlags Modifiers = ModifierFlags::None;
	bool bPressed = false;

	constexpr InputDevice GetDevice() const noexcept { return InputDevice::Mouse; }

	constexpr bool IsPressed() const noexcept { return bPressed; }

	constexpr bool IsReleased() const noexcept { return !bPressed; }

	SPARKLE_CORE_API bool HasModifier(ModifierFlags flag) const noexcept;

	constexpr bool IsLeftButton() const noexcept { return Button == MouseButton::Left; }

	constexpr bool IsRightButton() const noexcept { return Button == MouseButton::Right; }

	constexpr bool IsMiddleButton() const noexcept { return Button == MouseButton::Middle; }
};
