#include "PCH.h"
#include "Input/Backends/Win32InputBackend.h"
#include "Input/Backends/Win32KeyboardInput.h"
#include "Input/Backends/Win32MouseInput.h"
#include "Input/InputBackendResult.h"
#include "Window/Window.h"

InputBackendResult ProcessWin32InputMessage(const WindowMessageEvent& event)
{
	switch (event.msg)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			const auto keyboard = DecodeWin32Keyboard(event.wParam, event.lParam, event.msg == WM_KEYDOWN || event.msg == WM_SYSKEYDOWN);
			return {.Type = keyboard.KeyCode != Key::Unknown ? InputEventType::Keyboard : InputEventType::None, .Keyboard = keyboard};
		}
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			return {.Type = InputEventType::MouseButton, .MouseButton = DecodeWin32MouseButton(MouseButton::Left, event.msg == WM_LBUTTONDOWN, event.lParam)};
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			return {.Type = InputEventType::MouseButton, .MouseButton = DecodeWin32MouseButton(MouseButton::Right, event.msg == WM_RBUTTONDOWN, event.lParam)};
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			return {.Type = InputEventType::MouseButton, .MouseButton = DecodeWin32MouseButton(MouseButton::Middle, event.msg == WM_MBUTTONDOWN, event.lParam)};
		case WM_MOUSEMOVE:
			return {.Type = InputEventType::MouseMove, .MouseMove = DecodeWin32MouseMove(event.lParam)};
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			if (const auto wheel = DecodeWin32MouseWheel(event, event.msg == WM_MOUSEHWHEEL))
			{
				return {.Type = InputEventType::MouseWheel, .MouseWheel = *wheel};
			}
			return {};
		case WM_KILLFOCUS:
		case WM_DESTROY:
			return {.Type = InputEventType::FocusLost};
		case WM_ACTIVATEAPP:
			return {.Type = event.wParam == FALSE ? InputEventType::FocusLost : InputEventType::None};
		case WM_CANCELMODE:
			return {.Type = InputEventType::CaptureLost};
		case WM_CAPTURECHANGED:
			return {.Type = reinterpret_cast<HWND>(event.lParam) != event.hWnd ? InputEventType::CaptureLost : InputEventType::None};
		default:
			return {};
	}
}
