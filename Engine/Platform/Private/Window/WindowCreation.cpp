#include "PCH.h"
#include "Window/Window.h"

#include "Diagnostics/Logger.h"
#include "Diagnostics/Verify.h"
#include "Strings/StringUtils.h"

#include <dwmapi.h>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_platformLogger, "Platform");

void Window::InitializeNativeWindow(std::string_view title)
{
	const std::wstring wideTitle = Strings::ToWide(title);
	m_hInstance = GetModuleHandleW(nullptr);
	RegisterWindowClass();
	CreateWindowHandle(wideTitle.c_str());
}

void Window::DestroyNativeWindow() noexcept
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
	}
	if (m_windowClassAtom && m_hInstance)
	{
		UnregisterClassW(kWindowClassName, m_hInstance);
		m_windowClassAtom = 0;
	}
}

void Window::RegisterWindowClass()
{
	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = &Window::WindowProc;
	windowClass.hInstance = m_hInstance;
	windowClass.hIcon = LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
	windowClass.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
	windowClass.lpszClassName = kWindowClassName;
	windowClass.hIconSm = LoadIconW(nullptr, MAKEINTRESOURCEW(32512));

	m_windowClassAtom = RegisterClassExW(&windowClass);
	if (!m_windowClassAtom)
	{
		Diagnostics::Fatal(g_platformLogger, __FILE__, __LINE__, "Window: Failed to register window class");
	}
}

void Window::CreateWindowHandle(const wchar_t* title)
{
	m_hWnd = CreateWindowExW(kWindowedExStyle, kWindowClassName, title, kWindowedStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, m_hInstance, this);
	if (!m_hWnd)
	{
		Diagnostics::Fatal(g_platformLogger, __FILE__, __LINE__, "Window: Failed to create window");
	}

	const MARGINS margins{-1, -1, -1, -1};
	DwmExtendFrameIntoClientArea(m_hWnd, &margins);
}

void Window::ApplyInitialWindowState()
{
	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
}
