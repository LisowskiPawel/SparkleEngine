#include "PCH.h"

#include "Core/Public/Input/Keyboard/Key.h"

bool IsLetterKey(Key key) noexcept
{
	return key >= Key::A && key <= Key::Z;
}

bool IsNumberKey(Key key) noexcept
{
	return key >= Key::Num0 && key <= Key::Num9;
}

bool IsFunctionKey(Key key) noexcept
{
	return key >= Key::F1 && key <= Key::F12;
}

bool IsNumpadKey(Key key) noexcept
{
	return key >= Key::Numpad0 && key <= Key::NumpadDivide;
}

bool IsModifierKey(Key key) noexcept
{
	return key >= Key::LeftShift && key <= Key::RightSuper;
}

bool IsNavigationKey(Key key) noexcept
{
	return key >= Key::Up && key <= Key::Delete;
}
