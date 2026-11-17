#include "PCH.h"

#include "Core/Public/Input/Events/KeyboardEvent.h"
#include "Core/Public/Input/Events/MouseButtonEvent.h"
#include "Core/Public/Input/Events/MouseMoveEvent.h"

bool KeyboardEvent::HasModifier(ModifierFlags flag) const noexcept
{
	return HasAnyFlag(Modifiers, flag);
}

bool MouseButtonEvent::HasModifier(ModifierFlags flag) const noexcept
{
	return HasAnyFlag(Modifiers, flag);
}

bool MouseMoveEvent::HasModifier(ModifierFlags flag) const noexcept
{
	return HasAnyFlag(Modifiers, flag);
}
