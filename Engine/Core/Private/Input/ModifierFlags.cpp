#include "PCH.h"

#include "Core/Public/Input/Keyboard/ModifierFlags.h"

#include <type_traits>

ModifierFlags operator|(ModifierFlags lhs, ModifierFlags rhs) noexcept
{
	using Bits = std::underlying_type_t<ModifierFlags>;
	return static_cast<ModifierFlags>(static_cast<Bits>(lhs) | static_cast<Bits>(rhs));
}

ModifierFlags operator&(ModifierFlags lhs, ModifierFlags rhs) noexcept
{
	using Bits = std::underlying_type_t<ModifierFlags>;
	return static_cast<ModifierFlags>(static_cast<Bits>(lhs) & static_cast<Bits>(rhs));
}

ModifierFlags operator^(ModifierFlags lhs, ModifierFlags rhs) noexcept
{
	using Bits = std::underlying_type_t<ModifierFlags>;
	return static_cast<ModifierFlags>(static_cast<Bits>(lhs) ^ static_cast<Bits>(rhs));
}

ModifierFlags operator~(ModifierFlags flags) noexcept
{
	using Bits = std::underlying_type_t<ModifierFlags>;
	return static_cast<ModifierFlags>(~static_cast<Bits>(flags));
}

ModifierFlags& operator|=(ModifierFlags& lhs, ModifierFlags rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}

ModifierFlags& operator&=(ModifierFlags& lhs, ModifierFlags rhs) noexcept
{
	lhs = lhs & rhs;
	return lhs;
}

ModifierFlags& operator^=(ModifierFlags& lhs, ModifierFlags rhs) noexcept
{
	lhs = lhs ^ rhs;
	return lhs;
}

bool HasAllFlags(ModifierFlags flags, ModifierFlags test) noexcept
{
	return (flags & test) == test;
}

bool HasAnyFlag(ModifierFlags flags, ModifierFlags test) noexcept
{
	return (flags & test) != ModifierFlags::None;
}
