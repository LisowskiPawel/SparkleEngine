#pragma once

#include "Input/Events/KeyboardEvent.h"
#include "Input/Events/MouseButtonEvent.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"

#include <variant>

struct WindowMessageEvent;

struct InputFocusLost final
{
};

struct InputCaptureLost final
{
};

using Win32InputEvent = std::variant<std::monostate, KeyboardEvent, MouseButtonEvent, MouseMoveEvent, MouseWheelEvent, InputFocusLost, InputCaptureLost>;

Win32InputEvent ProcessWin32InputMessage(const WindowMessageEvent& event);
