#include "PCH.h"
#include "Input/SdlKeyboardInput.h"

#include <utility>

static constexpr std::pair<SDL_Keycode, Key> s_keyBindings[] = {
    {SDLK_UP, Key::Up},
    {SDLK_DOWN, Key::Down},
    {SDLK_LEFT, Key::Left},
    {SDLK_RIGHT, Key::Right},
    {SDLK_HOME, Key::Home},
    {SDLK_END, Key::End},
    {SDLK_PAGEUP, Key::PageUp},
    {SDLK_PAGEDOWN, Key::PageDown},
    {SDLK_INSERT, Key::Insert},
    {SDLK_DELETE, Key::Delete},
    {SDLK_LSHIFT, Key::LeftShift},
    {SDLK_RSHIFT, Key::RightShift},
    {SDLK_LCTRL, Key::LeftCtrl},
    {SDLK_RCTRL, Key::RightCtrl},
    {SDLK_LALT, Key::LeftAlt},
    {SDLK_RALT, Key::RightAlt},
    {SDLK_LGUI, Key::LeftSuper},
    {SDLK_RGUI, Key::RightSuper},
    {SDLK_SPACE, Key::Space},
    {SDLK_RETURN, Key::Enter},
    {SDLK_ESCAPE, Key::Escape},
    {SDLK_TAB, Key::Tab},
    {SDLK_BACKSPACE, Key::Backspace},
    {SDLK_CAPSLOCK, Key::CapsLock},
    {SDLK_NUMLOCKCLEAR, Key::NumLock},
    {SDLK_SCROLLLOCK, Key::ScrollLock},
    {SDLK_PRINTSCREEN, Key::PrintScreen},
    {SDLK_PAUSE, Key::Pause},
    {SDLK_COMMA, Key::Comma},
    {SDLK_PERIOD, Key::Period},
    {SDLK_SLASH, Key::Slash},
    {SDLK_SEMICOLON, Key::Semicolon},
    {SDLK_APOSTROPHE, Key::Apostrophe},
    {SDLK_LEFTBRACKET, Key::LeftBracket},
    {SDLK_RIGHTBRACKET, Key::RightBracket},
    {SDLK_BACKSLASH, Key::Backslash},
    {SDLK_GRAVE, Key::Grave},
    {SDLK_MINUS, Key::Minus},
    {SDLK_EQUALS, Key::Equals},
    {SDLK_KP_0, Key::Numpad0},
    {SDLK_KP_1, Key::Numpad1},
    {SDLK_KP_2, Key::Numpad2},
    {SDLK_KP_3, Key::Numpad3},
    {SDLK_KP_4, Key::Numpad4},
    {SDLK_KP_5, Key::Numpad5},
    {SDLK_KP_6, Key::Numpad6},
    {SDLK_KP_7, Key::Numpad7},
    {SDLK_KP_8, Key::Numpad8},
    {SDLK_KP_9, Key::Numpad9},
    {SDLK_KP_PERIOD, Key::NumpadDecimal},
    {SDLK_KP_ENTER, Key::NumpadEnter},
    {SDLK_KP_PLUS, Key::NumpadAdd},
    {SDLK_KP_MINUS, Key::NumpadSubtract},
    {SDLK_KP_MULTIPLY, Key::NumpadMultiply},
    {SDLK_KP_DIVIDE, Key::NumpadDivide},
};

static constexpr std::pair<SDL_Keymod, ModifierFlags> s_modifierBindings[] = {
    {SDL_KMOD_LSHIFT, ModifierFlags::LeftShift},
    {SDL_KMOD_RSHIFT, ModifierFlags::RightShift},
    {SDL_KMOD_LCTRL, ModifierFlags::LeftCtrl},
    {SDL_KMOD_RCTRL, ModifierFlags::RightCtrl},
    {SDL_KMOD_LALT, ModifierFlags::LeftAlt},
    {SDL_KMOD_RALT, ModifierFlags::RightAlt},
    {SDL_KMOD_LGUI, ModifierFlags::LeftSuper},
    {SDL_KMOD_RGUI, ModifierFlags::RightSuper},
    {SDL_KMOD_CAPS, ModifierFlags::CapsLock},
    {SDL_KMOD_NUM, ModifierFlags::NumLock},
    {SDL_KMOD_SCROLL, ModifierFlags::ScrollLock},
};

Key TranslateSdlKey(SDL_Keycode key) noexcept
{
	if (key >= SDLK_A && key <= SDLK_Z)
	{
		return static_cast<Key>(static_cast<unsigned>(Key::A) + key - SDLK_A);
	}
	if (key >= SDLK_0 && key <= SDLK_9)
	{
		return static_cast<Key>(static_cast<unsigned>(Key::Num0) + key - SDLK_0);
	}
	if (key >= SDLK_F1 && key <= SDLK_F12)
	{
		return static_cast<Key>(static_cast<unsigned>(Key::F1) + key - SDLK_F1);
	}
	for (const auto& [sdlKey, nativeKey] : s_keyBindings)
	{
		if (key == sdlKey)
		{
			return nativeKey;
		}
	}
	return Key::Unknown;
}

ModifierFlags TranslateSdlModifiers(SDL_Keymod modifiers) noexcept
{
	ModifierFlags result = ModifierFlags::None;
	for (const auto& [sdlFlag, nativeFlag] : s_modifierBindings)
	{
		if ((modifiers & sdlFlag) != 0)
		{
			result |= nativeFlag;
		}
	}
	return result;
}
