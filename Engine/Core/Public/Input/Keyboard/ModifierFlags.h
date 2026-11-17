#pragma once

#include <cstdint>
#include "Core/Public/CoreAPI.h"

enum class ModifierFlags : std::uint16_t
{
	None = 0,

	LeftShift = 1 << 0,
	RightShift = 1 << 1,
	LeftCtrl = 1 << 2,
	RightCtrl = 1 << 3,
	LeftAlt = 1 << 4,
	RightAlt = 1 << 5,
	LeftSuper = 1 << 6,
	RightSuper = 1 << 7,

	CapsLock = 1 << 8,
	NumLock = 1 << 9,
	ScrollLock = 1 << 10,

	Shift = LeftShift | RightShift,
	Ctrl = LeftCtrl | RightCtrl,
	Alt = LeftAlt | RightAlt,
	Super = LeftSuper | RightSuper,

	CtrlShift = Ctrl | Shift,
	CtrlAlt = Ctrl | Alt,
	ShiftAlt = Shift | Alt,
};

SPARKLE_CORE_API ModifierFlags operator|(ModifierFlags lhs, ModifierFlags rhs) noexcept;
SPARKLE_CORE_API ModifierFlags operator&(ModifierFlags lhs, ModifierFlags rhs) noexcept;
SPARKLE_CORE_API ModifierFlags operator^(ModifierFlags lhs, ModifierFlags rhs) noexcept;
SPARKLE_CORE_API ModifierFlags operator~(ModifierFlags flags) noexcept;

SPARKLE_CORE_API ModifierFlags& operator|=(ModifierFlags& lhs, ModifierFlags rhs) noexcept;
SPARKLE_CORE_API ModifierFlags& operator&=(ModifierFlags& lhs, ModifierFlags rhs) noexcept;
SPARKLE_CORE_API ModifierFlags& operator^=(ModifierFlags& lhs, ModifierFlags rhs) noexcept;

SPARKLE_CORE_API bool HasAllFlags(ModifierFlags flags, ModifierFlags test) noexcept;
SPARKLE_CORE_API bool HasAnyFlag(ModifierFlags flags, ModifierFlags test) noexcept;
