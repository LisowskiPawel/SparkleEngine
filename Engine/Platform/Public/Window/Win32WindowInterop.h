#pragma once

#include "Platform/Public/PlatformAPI.h"

class Window;

// Immutable borrowed HWND for graphics/capture threads. Retire those consumers before the window.
SPARKLE_PLATFORM_API void* GetWin32WindowHandle(const Window& window) noexcept;
