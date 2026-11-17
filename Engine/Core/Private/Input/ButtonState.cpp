#include "PCH.h"

#include "Core/Public/Input/State/ButtonState.h"

bool IsDown(ButtonState state) noexcept
{
	return state == ButtonState::Pressed || state == ButtonState::Held;
}

bool IsUp(ButtonState state) noexcept
{
	return state == ButtonState::Up || state == ButtonState::Released;
}

bool IsEdge(ButtonState state) noexcept
{
	return state == ButtonState::Pressed || state == ButtonState::Released;
}

bool IsContinuous(ButtonState state) noexcept
{
	return state == ButtonState::Up || state == ButtonState::Held;
}
