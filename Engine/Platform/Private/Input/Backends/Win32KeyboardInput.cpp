#include "PCH.h"
#include "Input/Backends/Win32KeyboardInput.h"

#include <Windows.h>

#include <array>

static constexpr auto s_virtualKeys = []
{
	std::array<Key, 256> keys{};
	auto mapRange = [&](UINT firstVirtualKey, Key firstKey, UINT count)
	{
		for (UINT index = 0; index < count; ++index)
		{
			keys[firstVirtualKey + index] = static_cast<Key>(static_cast<UINT>(firstKey) + index);
		}
	};
	mapRange('A', Key::A, 26);
	mapRange('0', Key::Num0, 10);
	mapRange(VK_F1, Key::F1, 12);
	mapRange(VK_NUMPAD0, Key::Numpad0, 10);

	keys[VK_BACK] = Key::Backspace;
	keys[VK_TAB] = Key::Tab;
	keys[VK_RETURN] = Key::Enter;
	keys[VK_PAUSE] = Key::Pause;
	keys[VK_CAPITAL] = Key::CapsLock;
	keys[VK_ESCAPE] = Key::Escape;
	keys[VK_SPACE] = Key::Space;
	keys[VK_PRIOR] = Key::PageUp;
	keys[VK_NEXT] = Key::PageDown;
	keys[VK_END] = Key::End;
	keys[VK_HOME] = Key::Home;
	keys[VK_LEFT] = Key::Left;
	keys[VK_UP] = Key::Up;
	keys[VK_RIGHT] = Key::Right;
	keys[VK_DOWN] = Key::Down;
	keys[VK_SNAPSHOT] = Key::PrintScreen;
	keys[VK_INSERT] = Key::Insert;
	keys[VK_DELETE] = Key::Delete;
	keys[VK_LWIN] = Key::LeftSuper;
	keys[VK_RWIN] = Key::RightSuper;
	keys[VK_LSHIFT] = Key::LeftShift;
	keys[VK_RSHIFT] = Key::RightShift;
	keys[VK_LCONTROL] = Key::LeftCtrl;
	keys[VK_RCONTROL] = Key::RightCtrl;
	keys[VK_LMENU] = Key::LeftAlt;
	keys[VK_RMENU] = Key::RightAlt;
	keys[VK_MULTIPLY] = Key::NumpadMultiply;
	keys[VK_ADD] = Key::NumpadAdd;
	keys[VK_SUBTRACT] = Key::NumpadSubtract;
	keys[VK_DECIMAL] = Key::NumpadDecimal;
	keys[VK_DIVIDE] = Key::NumpadDivide;
	keys[VK_OEM_1] = Key::Semicolon;
	keys[VK_OEM_PLUS] = Key::Equals;
	keys[VK_OEM_COMMA] = Key::Comma;
	keys[VK_OEM_MINUS] = Key::Minus;
	keys[VK_OEM_PERIOD] = Key::Period;
	keys[VK_OEM_2] = Key::Slash;
	keys[VK_OEM_3] = Key::Grave;
	keys[VK_OEM_4] = Key::LeftBracket;
	keys[VK_OEM_5] = Key::Backslash;
	keys[VK_OEM_6] = Key::RightBracket;
	keys[VK_OEM_7] = Key::Apostrophe;
	keys[VK_NUMLOCK] = Key::NumLock;
	keys[VK_SCROLL] = Key::ScrollLock;
	return keys;
}();

static Key TranslateVirtualKey(std::uintptr_t virtualKey, std::intptr_t flags) noexcept
{
	if (virtualKey == VK_SHIFT)
	{
		const UINT scanCode = (static_cast<std::uintptr_t>(flags) >> 16) & 0xff;
		return MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT ? Key::RightShift : Key::LeftShift;
	}

	const bool bExtendedKey = (flags & (std::intptr_t{1} << 24)) != 0;
	if (virtualKey == VK_CONTROL)
	{
		return bExtendedKey ? Key::RightCtrl : Key::LeftCtrl;
	}
	if (virtualKey == VK_MENU)
	{
		return bExtendedKey ? Key::RightAlt : Key::LeftAlt;
	}

	return virtualKey < s_virtualKeys.size() ? s_virtualKeys[virtualKey] : Key::Unknown;
}

ModifierFlags GetWin32InputModifiers() noexcept
{
	ModifierFlags modifiers = ModifierFlags::None;

	static constexpr std::pair<int, ModifierFlags> bindings[] = {
	    {VK_LSHIFT, ModifierFlags::LeftShift},
	    {VK_RSHIFT, ModifierFlags::RightShift},
	    {VK_LCONTROL, ModifierFlags::LeftCtrl},
	    {VK_RCONTROL, ModifierFlags::RightCtrl},
	    {VK_LMENU, ModifierFlags::LeftAlt},
	    {VK_RMENU, ModifierFlags::RightAlt},
	    {VK_LWIN, ModifierFlags::LeftSuper},
	    {VK_RWIN, ModifierFlags::RightSuper}};

	for (const auto& [virtualKey, flag] : bindings)
	{
		if ((GetKeyState(virtualKey) & 0x8000) != 0)
		{
			modifiers = modifiers | flag;
		}
	}

	if ((GetKeyState(VK_CAPITAL) & 1) != 0)
	{
		modifiers = modifiers | ModifierFlags::CapsLock;
	}
	if ((GetKeyState(VK_NUMLOCK) & 1) != 0)
	{
		modifiers = modifiers | ModifierFlags::NumLock;
	}
	if ((GetKeyState(VK_SCROLL) & 1) != 0)
	{
		modifiers = modifiers | ModifierFlags::ScrollLock;
	}
	return modifiers;
}

KeyboardEvent DecodeWin32Keyboard(std::uintptr_t virtualKey, std::intptr_t flags, bool bPressed) noexcept
{
	KeyboardEvent event{};
	event.KeyCode = TranslateVirtualKey(virtualKey, flags);
	event.Modifiers = GetWin32InputModifiers();
	event.bPressed = bPressed;
	event.bRepeat = bPressed && (flags & (std::intptr_t{1} << 30)) != 0;
	return event;
}
