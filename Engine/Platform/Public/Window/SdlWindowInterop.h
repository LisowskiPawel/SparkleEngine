#pragma once

#include "Platform/Public/PlatformAPI.h"
#include "Events/Event.h"
#include <cstdint>

class Window;
struct SDL_Window;
union SDL_Event;

// Borrowed only during synchronous delivery; SDL owns any text storage.
// Cancellation invalidates the remaining delivery of an interrupted input event.
struct SdlWindowEvent final
{
	const SDL_Event& Value;
	std::uint64_t CancellationEpoch;
	std::uint64_t PointerCancellationEpoch;
};

SPARKLE_PLATFORM_API SDL_Window* GetSdlWindow(const Window& window) noexcept;
SPARKLE_PLATFORM_API Event<void(const SdlWindowEvent&), 16>& GetSdlWindowEvents(Window& window) noexcept;
SPARKLE_PLATFORM_API bool IsSdlWindowEventCurrent(const Window& window, const SdlWindowEvent& event) noexcept;
SPARKLE_PLATFORM_API bool SetSdlWindowRelativeMode(Window& window, bool bEnabled) noexcept;
