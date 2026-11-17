#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>

enum class Key : std::uint8_t
{
	Unknown = 0,

	A = 1,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,

	Num0 = 27,
	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,

	F1 = 37,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	Up = 49,
	Down,
	Left,
	Right,
	Home,
	End,
	PageUp,
	PageDown,
	Insert,
	Delete,

	LeftShift = 59,
	RightShift,
	LeftCtrl,
	RightCtrl,
	LeftAlt,
	RightAlt,
	LeftSuper,
	RightSuper,

	Space = 67,
	Enter,
	Escape,
	Tab,
	Backspace,
	CapsLock,
	NumLock,
	ScrollLock,
	PrintScreen,
	Pause,

	Comma = 77,
	Period,
	Slash,
	Semicolon,
	Apostrophe,
	LeftBracket,
	RightBracket,
	Backslash,
	Grave,
	Minus,
	Equals,

	Numpad0 = 88,
	Numpad1,
	Numpad2,
	Numpad3,
	Numpad4,
	Numpad5,
	Numpad6,
	Numpad7,
	Numpad8,
	Numpad9,
	NumpadDecimal,
	NumpadEnter,
	NumpadAdd,
	NumpadSubtract,
	NumpadMultiply,
	NumpadDivide,

	Count
};

SPARKLE_CORE_API bool IsLetterKey(Key key) noexcept;
SPARKLE_CORE_API bool IsNumberKey(Key key) noexcept;
SPARKLE_CORE_API bool IsFunctionKey(Key key) noexcept;
SPARKLE_CORE_API bool IsNumpadKey(Key key) noexcept;
SPARKLE_CORE_API bool IsModifierKey(Key key) noexcept;
SPARKLE_CORE_API bool IsNavigationKey(Key key) noexcept;
