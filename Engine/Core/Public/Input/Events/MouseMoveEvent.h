#pragma once

#include "../../CoreAPI.h"
#include "../Mouse/MousePosition.h"
#include "../Keyboard/ModifierFlags.h"
#include "../Device/InputDevice.h"

struct MouseMoveEvent
{
	MousePosition Position;
	MouseDelta Delta;
	ModifierFlags Modifiers = ModifierFlags::None;

	constexpr InputDevice GetDevice() const noexcept { return InputDevice::Mouse; }

	SPARKLE_CORE_API bool HasModifier(ModifierFlags flag) const noexcept;
};
