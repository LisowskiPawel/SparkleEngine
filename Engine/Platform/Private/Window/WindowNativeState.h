#pragma once

#include "Window/Window.h"
#include "Window/SdlWindowInterop.h"
#include "Threading/ThreadOwnership.h"
#include <SDL3/SDL.h>
#include <Windows.h>
#include <optional>

struct Window::NativeState final
{
	explicit NativeState(Window& owner) noexcept;
	~NativeState();
	void Initialize(std::string_view title);
	void Shutdown() noexcept;
	static SDL_HitTestResult SDLCALL HitTest(SDL_Window* window, const SDL_Point* point, void* userData) noexcept;
	static LRESULT CALLBACK ObserveCancellation(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclassId, DWORD_PTR userData) noexcept;
	bool CheckOperation(bool bSucceeded, const char* operation) noexcept;

	Window& Owner;
	Threading::OwnerThread Thread{"Window and SDL"};
	SDL_Window* Handle = nullptr;
	HWND NativeHandle = nullptr;
	Event<void(const SdlWindowEvent&), 16> Events;
	std::uint64_t CancellationEpoch = 0;
	std::uint64_t PointerCancellationEpoch = 0;
	SDL_FRect TitleBar{};
	bool bPublishedMinimized = false;
	float DpiScale = 0.0f;
	std::optional<State> PendingShow;
	bool bVideoInitialized = false;
	bool bChangingRelativeMode = false;
	bool bObservingCancellation = false;
};
