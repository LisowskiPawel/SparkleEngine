#include "PCH.h"
#include "Input/Backends/Win32InputBackend.h"
#include "Input/Backends/Win32KeyboardInput.h"
#include "Input/Backends/Win32MouseInput.h"
#include "Window/Window.h"

Win32InputEvent ProcessWin32InputMessage(const WindowMessageEvent& event)
{
	switch (event.msg)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			const bool bPressed = event.msg == WM_KEYDOWN || event.msg == WM_SYSKEYDOWN;
			const auto keyboard = DecodeWin32Keyboard(event.wParam, event.lParam, bPressed);
			if (keyboard.KeyCode == Key::Unknown)
			{
				return {};
			}

			return keyboard;
		}
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			return DecodeWin32MouseButton(MouseButton::Left, event.msg == WM_LBUTTONDOWN, event.lParam);
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			return DecodeWin32MouseButton(MouseButton::Right, event.msg == WM_RBUTTONDOWN, event.lParam);
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			return DecodeWin32MouseButton(MouseButton::Middle, event.msg == WM_MBUTTONDOWN, event.lParam);
		case WM_MOUSEMOVE:
			return DecodeWin32MouseMove(event.lParam);
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			if (const auto wheel = DecodeWin32MouseWheel(event, event.msg == WM_MOUSEHWHEEL))
			{
				return *wheel;
			}

			return {};
		case WM_KILLFOCUS:
		case WM_DESTROY:
			return InputFocusLost{};
		case WM_ACTIVATEAPP:
			if (event.wParam == FALSE)
			{
				return InputFocusLost{};
			}

			return {};
		case WM_CANCELMODE:
			return InputCaptureLost{};
		case WM_CAPTURECHANGED:
			if (reinterpret_cast<HWND>(event.lParam) != event.hWnd)
			{
				return InputCaptureLost{};
			}

			return {};
		default:
			return {};
	}
}
