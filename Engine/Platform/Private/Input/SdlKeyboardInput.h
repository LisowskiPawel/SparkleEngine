#pragma once

#include "Input/Keyboard/Key.h"
#include "Input/Keyboard/ModifierFlags.h"
#include <SDL3/SDL_keycode.h>

Key TranslateSdlKey(SDL_Keycode key) noexcept;
ModifierFlags TranslateSdlModifiers(SDL_Keymod modifiers) noexcept;
