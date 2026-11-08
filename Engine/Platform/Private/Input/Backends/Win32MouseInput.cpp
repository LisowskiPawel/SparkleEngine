#include "PCH.h"
#include "Input/Backends/Win32MouseInput.h"
#include "Input/Backends/Win32KeyboardInput.h"
#include "Window/Window.h"

#include <windowsx.h>

MouseButtonEvent DecodeWin32MouseButton(MouseButton button, bool bPressed, std::intptr_t position) noexcept
{
	MouseButtonEvent event{};
	event.Button = button;
	event.bPressed = bPressed;
	event.Position = {GET_X_LPARAM(position), GET_Y_LPARAM(position)};
	event.Modifiers = GetWin32InputModifiers();
	return event;
}

MouseMoveEvent DecodeWin32MouseMove(std::intptr_t position) noexcept
{
	MouseMoveEvent event{};
	event.Position = {GET_X_LPARAM(position), GET_Y_LPARAM(position)};
	event.Modifiers = GetWin32InputModifiers();
	return event;
}

std::optional<MouseWheelEvent> DecodeWin32MouseWheel(const WindowMessageEvent& message, bool bHorizontal) noexcept
{
	POINT position{GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam)};
	if (message.hWnd == nullptr || !ScreenToClient(message.hWnd, &position))
	{
		return std::nullopt;
	}

	MouseWheelEvent event{};
	event.Position = {position.x, position.y};
	event.Delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(message.wParam)) / WHEEL_DELTA;
	event.bHorizontal = bHorizontal;
	return event;
}
