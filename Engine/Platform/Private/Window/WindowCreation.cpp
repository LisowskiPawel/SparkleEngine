#include "PCH.h"
#include "Window/WindowNativeState.h"
#include "Diagnostics/Logger.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_revision.h>
#include <commctrl.h>
#include <stdexcept>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_windowLogger, "Window");

static void RequireSdlOperation(bool bSucceeded, const char* operation)
{
	if (!bSucceeded)
	{
		throw std::runtime_error(std::string(operation) + ": " + SDL_GetError());
	}
}

Window::NativeState::NativeState(Window& owner) noexcept :
    Owner(owner)
{
}

void Window::NativeState::Initialize(std::string_view title)
{
	Thread.AssertAccess();
	const bool bMatchingVersion = SDL_GetVersion() == SDL_VERSION;
	const bool bMatchingRevision = std::string_view(SDL_GetRevision()) == SDL_REVISION;
	if (!bMatchingVersion || !bMatchingRevision)
	{
		throw std::runtime_error(std::string("SDL runtime does not match the compiled window backend. Expected ") + SDL_REVISION + "; loaded " + SDL_GetRevision());
	}
	SDL_SetMainReady();
	RequireSdlOperation(SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0"), "SDL disable automatic pointer capture");
	RequireSdlOperation(SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SYSTEM_SCALE, "1"), "SDL system-scaled relative motion");
	RequireSdlOperation(SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SPEED_SCALE, "1"), "SDL relative motion sensitivity");
	RequireSdlOperation(SDL_InitSubSystem(SDL_INIT_VIDEO), "SDL video initialization");
	bVideoInitialized = true;

	const std::string ownedTitle(title);
	const SDL_WindowFlags flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	Handle = SDL_CreateWindow(ownedTitle.c_str(), 1280, 800, flags);
	RequireSdlOperation(Handle != nullptr, "SDL native window creation");
	RequireSdlOperation(SDL_GetWindowDisplayScale(Handle) > 0.0f, "SDL initial display scale");
	const SDL_PropertiesID properties = SDL_GetWindowProperties(Handle);
	NativeHandle = static_cast<HWND>(SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
	RequireSdlOperation(NativeHandle != nullptr, "SDL borrowed Win32 presentation handle");
	RequireSdlOperation(SDL_SetWindowHitTest(Handle, HitTest, this), "SDL borderless hit testing");
	if (!SetWindowSubclass(NativeHandle, ObserveCancellation, 1, reinterpret_cast<DWORD_PTR>(this)))
	{
		throw std::runtime_error("Window cancellation observer installation failed: " + std::to_string(GetLastError()));
	}
	bObservingCancellation = true;
}

Window::NativeState::~NativeState()
{
	Shutdown();
}

void Window::NativeState::Shutdown() noexcept
{
	Thread.AssertAccess();
	if (bObservingCancellation)
	{
		RemoveWindowSubclass(NativeHandle, ObserveCancellation, 1);
		bObservingCancellation = false;
	}
	if (Handle != nullptr)
	{
		SDL_SetWindowHitTest(Handle, nullptr, nullptr);
		SDL_DestroyWindow(Handle);
		Handle = nullptr;
		NativeHandle = nullptr;
	}
	if (bVideoInitialized)
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		bVideoInitialized = false;
	}
}

bool Window::NativeState::CheckOperation(bool bSucceeded, const char* operation) noexcept
{
	if (!bSucceeded)
	{
		SPDLOG_LOGGER_ERROR(g_windowLogger, "{}: {}", operation, SDL_GetError());
	}
	return bSucceeded;
}
