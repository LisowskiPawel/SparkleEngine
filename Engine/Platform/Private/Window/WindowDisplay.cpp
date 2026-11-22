#include "PCH.h"
#include "Window/WindowNativeState.h"

#include <cmath>

float Window::GetDpiScale() const noexcept
{
	m_native->Thread.AssertAccess();
	return m_native->DpiScale;
}

bool Window::IsFullScreen() const noexcept
{
	return (SDL_GetWindowFlags(GetSdlWindow(*this)) & SDL_WINDOW_FULLSCREEN) != 0;
}

bool Window::IsMinimized() const noexcept
{
	return (SDL_GetWindowFlags(GetSdlWindow(*this)) & SDL_WINDOW_MINIMIZED) != 0;
}

bool Window::IsMaximized() const noexcept
{
	const SDL_WindowFlags flags = SDL_GetWindowFlags(GetSdlWindow(*this));
	return (flags & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_MAXIMIZED)) == SDL_WINDOW_MAXIMIZED;
}

void Window::ToggleFullScreen() noexcept
{
	const bool bFullScreen = IsFullScreen();
	m_native->CheckOperation(SDL_SetWindowFullscreen(m_native->Handle, !bFullScreen), "SDL fullscreen transition");
	RefreshNativeState();
}

void Window::Minimize() noexcept
{
	m_native->Thread.AssertAccess();
	m_native->PendingShow = NativeState::ShowCommand::Minimize;
}

void Window::Maximize() noexcept
{
	m_native->Thread.AssertAccess();
	m_native->PendingShow = NativeState::ShowCommand::Maximize;
}

void Window::Restore() noexcept
{
	m_native->Thread.AssertAccess();
	m_native->PendingShow = NativeState::ShowCommand::Restore;
}

void Window::ToggleMaximizeRestore() noexcept
{
	if (!IsFullScreen())
	{
		if (IsMaximized())
		{
			Restore();
		}
		else
		{
			Maximize();
		}
	}
}

void Window::ApplyPendingShowCommand() noexcept
{
	if (!m_native->PendingShow)
	{
		return;
	}
	const auto pending = *m_native->PendingShow;
	m_native->PendingShow.reset();
	switch (pending)
	{
		case NativeState::ShowCommand::Minimize:
			CancelInput(InputCancellation::All);
			m_native->CheckOperation(SDL_MinimizeWindow(m_native->Handle), "SDL minimize");
			break;
		case NativeState::ShowCommand::Maximize:
			m_native->CheckOperation(SDL_MaximizeWindow(m_native->Handle), "SDL maximize");
			break;
		default:
			m_native->CheckOperation(SDL_RestoreWindow(m_native->Handle), "SDL restore");
			break;
	}
	RefreshNativeState();
}

void Window::RefreshNativeState() noexcept
{
	const float dpiScale = SDL_GetWindowDisplayScale(m_native->Handle);
	const bool bDpiChanged = dpiScale > 0.0f && dpiScale != m_native->DpiScale;
	if (bDpiChanged)
	{
		m_native->DpiScale = dpiScale;
		const int minimumWidth = static_cast<int>(std::lround(320.0f * dpiScale));
		const int minimumHeight = static_cast<int>(std::lround(240.0f * dpiScale));
		m_native->CheckOperation(SDL_SetWindowMinimumSize(m_native->Handle, minimumWidth, minimumHeight), "SDL minimum window extent");
	}
	int width = 0;
	int height = 0;
	if (!m_native->CheckOperation(SDL_GetWindowSizeInPixels(m_native->Handle, &width, &height), "SDL drawable dimensions"))
	{
		RequestClose();
		return;
	}
	const auto pixelWidth = static_cast<uint32_t>(std::max(width, 0));
	const auto pixelHeight = static_cast<uint32_t>(std::max(height, 0));
	const bool bExtentChanged = GetWidth() != pixelWidth || GetHeight() != pixelHeight;
	const SDL_WindowFlags flags = SDL_GetWindowFlags(m_native->Handle);
	const bool bMinimized = (flags & SDL_WINDOW_MINIMIZED) != 0;
	const bool bVisibilityChanged = bMinimized != m_native->bPublishedMinimized;
	m_clientWidth.store(pixelWidth, std::memory_order_release);
	m_clientHeight.store(pixelHeight, std::memory_order_release);
	m_native->bPublishedMinimized = bMinimized;
	if (bExtentChanged || bDpiChanged)
	{
		m_native->TitleBar = {};
	}
	if (bDpiChanged)
	{
		OnDpiScaleChanged.Broadcast(dpiScale);
	}
	if (bExtentChanged || bVisibilityChanged)
	{
		OnResized.Broadcast();
	}
}
