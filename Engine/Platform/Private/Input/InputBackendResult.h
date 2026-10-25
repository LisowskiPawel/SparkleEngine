#pragma once

#include "Input/Events/KeyboardEvent.h"
#include "Input/Events/MouseButtonEvent.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"

#include <cstdint>

enum class InputEventType : std::uint8_t
{
	None,
	Keyboard,
	MouseButton,
	MouseMove,
	MouseWheel
};

struct InputBackendResult
{
	InputEventType Type = InputEventType::None;

	union
	{
		KeyboardEvent Keyboard{};
		MouseButtonEvent MouseButton;
		MouseMoveEvent MouseMove;
		MouseWheelEvent MouseWheel;
	};

	bool IsValid() const noexcept { return Type != InputEventType::None; }
};
