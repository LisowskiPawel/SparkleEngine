#include "PCH.h"
#include "Window/Window.h"

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = nullptr;
	if (msg == WM_NCCREATE)
	{
		const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lParam);
		window = static_cast<Window*>(create->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		window->m_hWnd = hWnd;
	}
	else
	{
		window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}
	if (window == nullptr)
	{
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	const LRESULT result = window->HandleMessage(msg, wParam, lParam);
	if (msg == WM_NCDESTROY)
	{
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
		window->m_hWnd = nullptr;
		window->RequestClose();
	}
	return result;
}

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowMessageEvent event{m_hWnd, msg, wParam, lParam, false};
	OnWindowMessage.Broadcast(event);
	// Subscribers can consume default handling, but cannot veto native state changes.
	switch (msg)
	{
		case WM_SIZE:
			OnSizeChanged(wParam, lParam);
			return 0;
		case WM_DPICHANGED:
			HandleDpiChanged(wParam, lParam);
			return 0;
		case WM_DESTROY:
			RequestClose();
			return 0;
	}

	if (event.bHandled)
	{
		return 0;
	}

	switch (msg)
	{
		case WM_NCCALCSIZE:
			return HandleNonClientSize(wParam, lParam);
		case WM_NCHITTEST:
			return HandleResizeHitTest(wParam, lParam);
		case WM_GETMINMAXINFO:
			return HandleTrackSizeConstraints(lParam);
		case WM_CLOSE:
			RequestClose();
			return 0;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (HandleWindowShortcut(msg, wParam, lParam))
			{
				return 0;
			}

			break;
		case WM_ERASEBKGND:
			return 1;
	}

	return DefWindowProcW(event.hWnd, msg, wParam, lParam);
}

void Window::PumpNativeMessages() noexcept
{
	MSG message{};
	while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
	{
		if (message.message == WM_QUIT)
		{
			RequestClose();
			continue;
		}
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}

void Window::WaitForEvent() noexcept
{
	if (m_hWnd == nullptr || m_bShouldClose)
	{
		return;
	}
	WaitMessage();
}
