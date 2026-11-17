#pragma once

#include "../../CoreAPI.h"
#include "../Keyboard/Key.h"
#include "../Keyboard/ModifierFlags.h"
#include "../Device/InputDevice.h"

struct KeyboardEvent
{
	Key KeyCode = Key::Unknown;
	ModifierFlags Modifiers = ModifierFlags::None;
	bool bPressed = false;
	bool bRepeat = false;

	constexpr InputDevice GetDevice() const noexcept { return InputDevice::Keyboard; }

	constexpr bool IsPressed() const noexcept { return bPressed; }

	constexpr bool IsReleased() const noexcept { return !bPressed; }

	SPARKLE_CORE_API bool HasModifier(ModifierFlags flag) const noexcept;
};
