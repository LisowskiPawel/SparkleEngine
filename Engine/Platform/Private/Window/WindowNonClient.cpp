#include "PCH.h"
#include "Window/WindowNativeState.h"

void Window::SetTitleBarDragRegion(float left, float top, float right, float bottom) noexcept
{
	m_native->Thread.AssertAccess();
	m_native->TitleBar = SDL_FRect{left, top, std::max(right - left, 0.0f), std::max(bottom - top, 0.0f)};
}

SDL_HitTestResult SDLCALL Window::NativeState::HitTest(SDL_Window*, const SDL_Point* point, void* userData) noexcept
{
	const auto& native = *static_cast<NativeState*>(userData);
	const Window& owner = native.Owner;
	if (owner.IsFullScreen())
	{
		return SDL_HITTEST_NORMAL;
	}
	if (!owner.IsMaximized())
	{
		const UINT dpi = GetDpiForWindow(native.NativeHandle);
		const int border = GetSystemMetricsForDpi(SM_CXFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
		const bool bLeft = point->x < border;
		const bool bRight = point->x >= static_cast<int>(owner.GetWidth()) - border;
		const bool bTop = point->y < border;
		const bool bBottom = point->y >= static_cast<int>(owner.GetHeight()) - border;
		if (bTop)
		{
			return bLeft ? SDL_HITTEST_RESIZE_TOPLEFT : bRight ? SDL_HITTEST_RESIZE_TOPRIGHT : SDL_HITTEST_RESIZE_TOP;
		}
		if (bBottom)
		{
			return bLeft ? SDL_HITTEST_RESIZE_BOTTOMLEFT : bRight ? SDL_HITTEST_RESIZE_BOTTOMRIGHT : SDL_HITTEST_RESIZE_BOTTOM;
		}
		if (bLeft || bRight)
		{
			return bLeft ? SDL_HITTEST_RESIZE_LEFT : SDL_HITTEST_RESIZE_RIGHT;
		}
	}
	const SDL_FPoint position{static_cast<float>(point->x), static_cast<float>(point->y)};
	return SDL_PointInRectFloat(&position, &native.TitleBar) ? SDL_HITTEST_DRAGGABLE : SDL_HITTEST_NORMAL;
}
