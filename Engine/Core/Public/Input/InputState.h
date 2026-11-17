#pragma once

#include "Core/Public/CoreAPI.h"

#include "Keyboard/Key.h"
#include "Keyboard/ModifierFlags.h"
#include "Mouse/MouseButton.h"
#include "Mouse/MousePosition.h"
#include "State/ButtonState.h"

#include <cstddef>
#include <array>
#include <cstdint>
#include <span>

class SPARKLE_CORE_API InputState final
{
public:
	InputState();

	~InputState();

	InputState(const InputState&) = delete;
	InputState& operator=(const InputState&) = delete;

	InputState(InputState&&) noexcept;
	InputState& operator=(InputState&&) noexcept;

	bool IsKeyDown(Key key) const noexcept;
	bool IsKeyPressed(Key key) const noexcept;
	bool IsKeyReleased(Key key) const noexcept;
	bool IsKeyHeld(Key key) const noexcept;
	ButtonState GetKeyState(Key key) const noexcept;

	bool IsMouseButtonDown(MouseButton button) const noexcept;
	bool IsMouseButtonPressed(MouseButton button) const noexcept;
	bool IsMouseButtonReleased(MouseButton button) const noexcept;
	bool IsMouseButtonHeld(MouseButton button) const noexcept;
	ButtonState GetMouseButtonState(MouseButton button) const noexcept;

	MousePosition GetMousePosition() const noexcept;
	MouseDelta GetMouseDelta() const noexcept;
	float GetMouseWheelDelta() const noexcept;
	float GetMouseWheelHorizontalDelta() const noexcept;

	ModifierFlags GetModifiers() const noexcept;
	bool HasModifier(ModifierFlags modifier) const noexcept;
	bool IsShiftDown() const noexcept;
	bool IsCtrlDown() const noexcept;
	bool IsAltDown() const noexcept;

	bool IsMouseCaptured() const noexcept;
	bool IsCursorHidden() const noexcept;

	// The acquisition owner updates state between observed frames; readers borrow const state.
	void SetKeyState(Key key, ButtonState state) noexcept;
	void SetMouseButtonState(MouseButton button, ButtonState state) noexcept;

	void SetMousePosition(std::int32_t x, std::int32_t y) noexcept;
	void AccumulateMouseDelta(std::int32_t deltaX, std::int32_t deltaY) noexcept;
	void AccumulateWheelDelta(float delta) noexcept;
	void AccumulateWheelHorizontalDelta(float delta) noexcept;

	void SetModifiers(ModifierFlags modifiers) noexcept;
	void SetMouseCaptured(bool bCaptured) noexcept;
	void SetCursorHidden(bool bHidden) noexcept;

	void BeginFrame() noexcept;
	void ResetPointerState() noexcept;

private:
	static void AdvanceButtonStates(std::span<ButtonState> states) noexcept;
	void ResetFrameDeltas() noexcept;

	static constexpr std::size_t kKeyCount = static_cast<std::size_t>(Key::Count);
	static constexpr std::size_t kMouseButtonCount = static_cast<std::size_t>(MouseButton::Count);

	std::array<ButtonState, kKeyCount> m_keyStates{};
	std::array<ButtonState, kMouseButtonCount> m_mouseButtonStates{};

	MousePosition m_mousePosition;
	MouseDelta m_mouseDelta;
	float m_wheelDelta = 0.0f;
	float m_wheelHorizontalDelta = 0.0f;

	ModifierFlags m_modifiers = ModifierFlags::None;
	bool m_bMouseCaptured = false;
	bool m_bCursorHidden = false;
};
