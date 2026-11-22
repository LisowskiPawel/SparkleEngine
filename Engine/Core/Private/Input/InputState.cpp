#include "PCH.h"
#include "Core/Public/Input/InputState.h"

InputState::InputState() = default;

InputState::~InputState() = default;

InputState::InputState(InputState&&) noexcept = default;

InputState& InputState::operator=(InputState&&) noexcept = default;

bool InputState::IsKeyDown(Key key) const noexcept
{
	return IsDown(GetKeyState(key));
}

bool InputState::IsKeyPressed(Key key) const noexcept
{
	return GetKeyState(key) == ButtonState::Pressed;
}

bool InputState::IsKeyReleased(Key key) const noexcept
{
	return GetKeyState(key) == ButtonState::Released;
}

bool InputState::IsKeyHeld(Key key) const noexcept
{
	return GetKeyState(key) == ButtonState::Held;
}

ButtonState InputState::GetKeyState(Key key) const noexcept
{
	const std::size_t index = static_cast<std::size_t>(key);
	if (index >= kKeyCount)
	{
		return ButtonState::Up;
	}

	return m_keyStates[index];
}

bool InputState::IsMouseButtonDown(MouseButton button) const noexcept
{
	return IsDown(GetMouseButtonState(button));
}

bool InputState::IsMouseButtonPressed(MouseButton button) const noexcept
{
	return GetMouseButtonState(button) == ButtonState::Pressed;
}

bool InputState::IsMouseButtonReleased(MouseButton button) const noexcept
{
	return GetMouseButtonState(button) == ButtonState::Released;
}

bool InputState::IsMouseButtonHeld(MouseButton button) const noexcept
{
	return GetMouseButtonState(button) == ButtonState::Held;
}

ButtonState InputState::GetMouseButtonState(MouseButton button) const noexcept
{
	const std::size_t index = static_cast<std::size_t>(button);
	if (index >= kMouseButtonCount)
	{
		return ButtonState::Up;
	}

	return m_mouseButtonStates[index];
}

MousePosition InputState::GetMousePosition() const noexcept
{
	return m_mousePosition;
}

MouseDelta InputState::GetMouseDelta() const noexcept
{
	return m_mouseDelta;
}

float InputState::GetMouseWheelDelta() const noexcept
{
	return m_wheelDelta;
}

float InputState::GetMouseWheelHorizontalDelta() const noexcept
{
	return m_wheelHorizontalDelta;
}

ModifierFlags InputState::GetModifiers() const noexcept
{
	return m_modifiers;
}

bool InputState::HasModifier(ModifierFlags modifier) const noexcept
{
	return HasAnyFlag(m_modifiers, modifier);
}

bool InputState::IsShiftDown() const noexcept
{
	return HasModifier(ModifierFlags::Shift);
}

bool InputState::IsCtrlDown() const noexcept
{
	return HasModifier(ModifierFlags::Ctrl);
}

bool InputState::IsAltDown() const noexcept
{
	return HasModifier(ModifierFlags::Alt);
}

bool InputState::IsMouseCaptured() const noexcept
{
	return m_bMouseCaptured;
}

bool InputState::IsCursorHidden() const noexcept
{
	return m_bCursorHidden;
}

void InputState::SetKeyState(Key key, ButtonState state) noexcept
{
	const std::size_t index = static_cast<std::size_t>(key);
	if (index < kKeyCount)
	{
		m_keyStates[index] = state;
	}
}

void InputState::SetMouseButtonState(MouseButton button, ButtonState state) noexcept
{
	const std::size_t index = static_cast<std::size_t>(button);
	if (index < kMouseButtonCount)
	{
		m_mouseButtonStates[index] = state;
	}
}

void InputState::SetMousePosition(float x, float y) noexcept
{
	m_mousePosition.X = x;
	m_mousePosition.Y = y;
}

void InputState::AccumulateMouseDelta(float deltaX, float deltaY) noexcept
{
	m_mouseDelta.X += deltaX;
	m_mouseDelta.Y += deltaY;
}

void InputState::AccumulateWheelDelta(float delta) noexcept
{
	m_wheelDelta += delta;
}

void InputState::AccumulateWheelHorizontalDelta(float delta) noexcept
{
	m_wheelHorizontalDelta += delta;
}

void InputState::SetModifiers(ModifierFlags modifiers) noexcept
{
	m_modifiers = modifiers;
}

void InputState::SetMouseCaptured(bool bCaptured) noexcept
{
	m_bMouseCaptured = bCaptured;
}

void InputState::SetCursorHidden(bool bHidden) noexcept
{
	m_bCursorHidden = bHidden;
}

void InputState::BeginFrame() noexcept
{
	AdvanceButtonStates(m_keyStates);
	AdvanceButtonStates(m_mouseButtonStates);
	ResetFrameDeltas();
}

void InputState::ResetPointerState() noexcept
{
	m_mouseButtonStates.fill(ButtonState::Up);
	ResetFrameDeltas();
	m_bMouseCaptured = false;
	m_bCursorHidden = false;
}

void InputState::AdvanceButtonStates(std::span<ButtonState> states) noexcept
{
	for (ButtonState& state : states)
	{
		if (state == ButtonState::Pressed)
		{
			state = ButtonState::Held;
		}
		else if (state == ButtonState::Released)
		{
			state = ButtonState::Up;
		}
	}
}

void InputState::ResetFrameDeltas() noexcept
{
	m_mouseDelta.X = 0;
	m_mouseDelta.Y = 0;
	m_wheelDelta = 0.0f;
	m_wheelHorizontalDelta = 0.0f;
}
