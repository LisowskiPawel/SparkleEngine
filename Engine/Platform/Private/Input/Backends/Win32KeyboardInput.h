#pragma once

#include "Input/Events/KeyboardEvent.h"

#include <cstdint>

KeyboardEvent DecodeWin32Keyboard(std::uintptr_t virtualKey, std::intptr_t flags, bool bPressed) noexcept;
ModifierFlags GetWin32InputModifiers() noexcept;
