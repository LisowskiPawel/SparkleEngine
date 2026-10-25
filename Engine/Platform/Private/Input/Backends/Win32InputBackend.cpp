#include "PCH.h"
#include "Input/Backends/Win32InputBackend.h"
#include "Input/InputBackendResult.h"
#include "Input/Keyboard/Key.h"

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <windowsx.h>

static constexpr Key s_VirtualKeyToKey[] = {
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Backspace,
    Key::Tab,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Enter,
    Key::Unknown,
    Key::Unknown,
    Key::LeftShift,
    Key::LeftCtrl,
    Key::LeftAlt,
    Key::Pause,
    Key::CapsLock,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Escape,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Space,
    Key::PageUp,
    Key::PageDown,
    Key::End,
    Key::Home,
    Key::Left,
    Key::Up,
    Key::Right,
    Key::Down,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::PrintScreen,
    Key::Insert,
    Key::Delete,
    Key::Unknown,
    Key::Num0,
    Key::Num1,
    Key::Num2,
    Key::Num3,
    Key::Num4,
    Key::Num5,
    Key::Num6,
    Key::Num7,
    Key::Num8,
    Key::Num9,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::A,
    Key::B,
    Key::C,
    Key::D,
    Key::E,
    Key::F,
    Key::G,
    Key::H,
    Key::I,
    Key::J,
    Key::K,
    Key::L,
    Key::M,
    Key::N,
    Key::O,
    Key::P,
    Key::Q,
    Key::R,
    Key::S,
    Key::T,
    Key::U,
    Key::V,
    Key::W,
    Key::X,
    Key::Y,
    Key::Z,
    Key::LeftSuper,
    Key::RightSuper,
    Key::Unknown,
    Key::Unknown,
    Key::Unknown,
    Key::Numpad0,
    Key::Numpad1,
    Key::Numpad2,
    Key::Numpad3,
    Key::Numpad4,
    Key::Numpad5,
    Key::Numpad6,
    Key::Numpad7,
    Key::Numpad8,
    Key::Numpad9,
    Key::NumpadMultiply,
    Key::NumpadAdd,
    Key::Unknown,
    Key::NumpadSubtract,
    Key::NumpadDecimal,
    Key::NumpadDivide,
    Key::F1,
    Key::F2,
    Key::F3,
    Key::F4,
    Key::F5,
    Key::F6,
    Key::F7,
    Key::F8,
    Key::F9,
    Key::F10,
    Key::F11,
    Key::F12,
};

static constexpr size_t s_VirtualKeyTableSize = sizeof(s_VirtualKeyToKey) / sizeof(s_VirtualKeyToKey[0]);

static Key TranslateVirtualKey(WPARAM VirtualKey) noexcept
{
	if (VirtualKey == VK_SHIFT)
	{
		if (GetKeyState(VK_LSHIFT) & 0x8000)
		{
			return Key::LeftShift;
		}
		if (GetKeyState(VK_RSHIFT) & 0x8000)
		{
			return Key::RightShift;
		}
		return Key::LeftShift;
	}
	if (VirtualKey == VK_CONTROL)
	{
		if (GetKeyState(VK_LCONTROL) & 0x8000)
		{
			return Key::LeftCtrl;
		}
		if (GetKeyState(VK_RCONTROL) & 0x8000)
		{
			return Key::RightCtrl;
		}
		return Key::LeftCtrl;
	}
	if (VirtualKey == VK_MENU)
	{
		if (GetKeyState(VK_LMENU) & 0x8000)
		{
			return Key::LeftAlt;
		}
		if (GetKeyState(VK_RMENU) & 0x8000)
		{
			return Key::RightAlt;
		}
		return Key::LeftAlt;
	}

	if (VirtualKey == VK_LSHIFT)
	{
		return Key::LeftShift;
	}
	if (VirtualKey == VK_RSHIFT)
	{
		return Key::RightShift;
	}
	if (VirtualKey == VK_LCONTROL)
	{
		return Key::LeftCtrl;
	}
	if (VirtualKey == VK_RCONTROL)
	{
		return Key::RightCtrl;
	}
	if (VirtualKey == VK_LMENU)
	{
		return Key::LeftAlt;
	}
	if (VirtualKey == VK_RMENU)
	{
		return Key::RightAlt;
	}

	if (VirtualKey == VK_RETURN)
	{
		return Key::Enter;
	}

	if (VirtualKey < s_VirtualKeyTableSize)
	{
		return s_VirtualKeyToKey[VirtualKey];
	}

	switch (VirtualKey)
	{
		case VK_OEM_1:
			return Key::Semicolon;
		case VK_OEM_PLUS:
			return Key::Equals;
		case VK_OEM_COMMA:
			return Key::Comma;
		case VK_OEM_MINUS:
			return Key::Minus;
		case VK_OEM_PERIOD:
			return Key::Period;
		case VK_OEM_2:
			return Key::Slash;
		case VK_OEM_3:
			return Key::Grave;
		case VK_OEM_4:
			return Key::LeftBracket;
		case VK_OEM_5:
			return Key::Backslash;
		case VK_OEM_6:
			return Key::RightBracket;
		case VK_OEM_7:
			return Key::Apostrophe;
		case VK_NUMLOCK:
			return Key::NumLock;
		case VK_SCROLL:
			return Key::ScrollLock;
		default:
			return Key::Unknown;
	}
}

static ModifierFlags GetCurrentModifiers() noexcept
{
	ModifierFlags Flags = ModifierFlags::None;

	if (GetKeyState(VK_LSHIFT) & 0x8000)
	{
		Flags = Flags | ModifierFlags::LeftShift;
	}
	if (GetKeyState(VK_RSHIFT) & 0x8000)
	{
		Flags = Flags | ModifierFlags::RightShift;
	}
	if (GetKeyState(VK_LCONTROL) & 0x8000)
	{
		Flags = Flags | ModifierFlags::LeftCtrl;
	}
	if (GetKeyState(VK_RCONTROL) & 0x8000)
	{
		Flags = Flags | ModifierFlags::RightCtrl;
	}
	if (GetKeyState(VK_LMENU) & 0x8000)
	{
		Flags = Flags | ModifierFlags::LeftAlt;
	}
	if (GetKeyState(VK_RMENU) & 0x8000)
	{
		Flags = Flags | ModifierFlags::RightAlt;
	}
	if (GetKeyState(VK_CAPITAL) & 0x0001)
	{
		Flags = Flags | ModifierFlags::CapsLock;
	}
	if (GetKeyState(VK_NUMLOCK) & 0x0001)
	{
		Flags = Flags | ModifierFlags::NumLock;
	}

	return Flags;
}

InputBackendResult ProcessWin32InputMessage(std::uint32_t message, std::uintptr_t parameter1, std::intptr_t parameter2)
{
	InputBackendResult result{};
	WPARAM WParam = static_cast<WPARAM>(parameter1);
	LPARAM LParam = static_cast<LPARAM>(parameter2);

	switch (message)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			result.Type = InputEventType::Keyboard;
			result.Keyboard.KeyCode = TranslateVirtualKey(WParam);
			result.Keyboard.bPressed = true;
			result.Keyboard.bRepeat = (LParam & 0x40000000) != 0;
			result.Keyboard.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			result.Type = InputEventType::Keyboard;
			result.Keyboard.KeyCode = TranslateVirtualKey(WParam);
			result.Keyboard.bPressed = false;
			result.Keyboard.bRepeat = false;
			result.Keyboard.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_LBUTTONDOWN:
		{
			result.Type = InputEventType::MouseButton;
			result.MouseButton.Button = MouseButton::Left;
			result.MouseButton.bPressed = true;
			result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_LBUTTONUP:
		{
			result.Type = InputEventType::MouseButton;
			result.MouseButton.Button = MouseButton::Left;
			result.MouseButton.bPressed = false;
			result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_RBUTTONDOWN:
		{
			result.Type = InputEventType::MouseButton;
			result.MouseButton.Button = MouseButton::Right;
			result.MouseButton.bPressed = true;
			result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_RBUTTONUP:
		{
			result.Type = InputEventType::MouseButton;
			result.MouseButton.Button = MouseButton::Right;
			result.MouseButton.bPressed = false;
			result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_MBUTTONDOWN:
		{
			result.Type = InputEventType::MouseButton;
			result.MouseButton.Button = MouseButton::Middle;
			result.MouseButton.bPressed = true;
			result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_MBUTTONUP:
		{
			result.Type = InputEventType::MouseButton;
			result.MouseButton.Button = MouseButton::Middle;
			result.MouseButton.bPressed = false;
			result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_MOUSEMOVE:
		{
			result.Type = InputEventType::MouseMove;
			result.MouseMove.Position.X = GET_X_LPARAM(LParam);
			result.MouseMove.Position.Y = GET_Y_LPARAM(LParam);

			result.MouseMove.Delta.X = 0;
			result.MouseMove.Delta.Y = 0;
			break;
		}

		case WM_MOUSEWHEEL:
		{
			result.Type = InputEventType::MouseWheel;
			result.MouseWheel.Delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(WParam)) / WHEEL_DELTA;
			result.MouseWheel.bHorizontal = false;
			result.MouseWheel.Position.X = GET_X_LPARAM(LParam);
			result.MouseWheel.Position.Y = GET_Y_LPARAM(LParam);
			break;
		}

		case WM_MOUSEHWHEEL:
		{
			result.Type = InputEventType::MouseWheel;
			result.MouseWheel.Delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(WParam)) / WHEEL_DELTA;
			result.MouseWheel.bHorizontal = true;
			result.MouseWheel.Position.X = GET_X_LPARAM(LParam);
			result.MouseWheel.Position.Y = GET_Y_LPARAM(LParam);
			break;
		}

		default:

			break;
	}

	return result;
}
