#pragma once

#include "Input/Events/MouseButtonEvent.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"

#include <cstdint>
#include <optional>

struct WindowMessageEvent;

MouseButtonEvent DecodeWin32MouseButton(MouseButton button, bool bPressed, std::intptr_t position) noexcept;
MouseMoveEvent DecodeWin32MouseMove(std::intptr_t position) noexcept;
std::optional<MouseWheelEvent> DecodeWin32MouseWheel(const WindowMessageEvent& event, bool bHorizontal) noexcept;
