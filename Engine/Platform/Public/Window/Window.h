#pragma once

#include "Platform/Public/PlatformAPI.h"
#include "Events/Event.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <string_view>

class Window;
struct SDL_Window;
struct SdlWindowEvent;

enum class InputCancellation : std::uint8_t
{
	Pointer,
	All
};

// Native queries, mutation and synchronous callbacks belong to the creating main thread.
// Drawable dimensions alone are published for rendering-thread readers.
class SPARKLE_PLATFORM_API Window final
{
public:
	explicit Window(std::string_view windowTitle);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	Event<void()> OnResized;
	Event<void(float)> OnDpiScaleChanged;
	Event<void(InputCancellation)> OnInputCanceled;

	void PollEvents() noexcept;
	void WaitForEvent() noexcept;

	uint32_t GetWidth() const noexcept { return m_clientWidth.load(std::memory_order_acquire); }

	uint32_t GetHeight() const noexcept { return m_clientHeight.load(std::memory_order_acquire); }

	float GetDpiScale() const noexcept;
	bool HasValidSize() const noexcept;

	bool ShouldClose() const noexcept { return m_bShouldClose; }

	bool IsFullScreen() const noexcept;
	bool IsMinimized() const noexcept;
	bool IsMaximized() const noexcept;
	void Minimize() noexcept;
	void Maximize() noexcept;
	void Restore() noexcept;
	void ToggleMaximizeRestore() noexcept;
	void RequestClose() noexcept;

	// Client coordinates of the blank title strip, excluding menus and window buttons.
	void SetTitleBarDragRegion(float left, float top, float right, float bottom) noexcept;

private:
	friend SPARKLE_PLATFORM_API void* GetWin32WindowHandle(const Window&) noexcept;
	friend SPARKLE_PLATFORM_API SDL_Window* GetSdlWindow(const Window&) noexcept;
	friend SPARKLE_PLATFORM_API Event<void(const SdlWindowEvent&), 16>& GetSdlWindowEvents(Window&) noexcept;
	friend SPARKLE_PLATFORM_API bool IsSdlWindowEventCurrent(const Window&, const SdlWindowEvent&) noexcept;
	friend SPARKLE_PLATFORM_API bool SetSdlWindowRelativeMode(Window&, bool) noexcept;

	struct NativeState;
	void RefreshNativeState() noexcept;
	void CancelInput(InputCancellation cancellation) noexcept;
	void ApplyPendingShowCommand() noexcept;
	void ToggleFullScreen() noexcept;

	std::unique_ptr<NativeState> m_native;
	std::atomic<uint32_t> m_clientWidth{0};
	std::atomic<uint32_t> m_clientHeight{0};
	bool m_bShouldClose = false;
};
