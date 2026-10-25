#include "PCH.h"
#include "ImGui/EditorImGuiContext.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Style/SparkleUiTheme.h"
#include "Window/Window.h"

#include <backends/imgui_impl_win32.h>
#include <imgui.h>

#include <utility>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_editorLogger, "Editor");

EditorImGuiContext::EditorImGuiContext(Window& window) noexcept :
	m_window(window)
{
}

EditorImGuiContext::~EditorImGuiContext() noexcept
{
	UnsubscribeFromWindowEvents();
	if (m_platformInitialized)
	{
		ImGui_ImplWin32_Shutdown();
		m_platformInitialized = false;
	}
	if (m_context != nullptr)
	{
		ImGui::DestroyContext(m_context);
		m_context = nullptr;
	}
}

void EditorImGuiContext::Initialize()
{
	IMGUI_CHECKVERSION();
	m_context = ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	m_iniPath = (userState.SettingsRoot / "EditorLayout.ini").string();
	io.IniFilename = m_iniPath.c_str();

}

bool EditorImGuiContext::InitializePlatformBackend()
{
	if (!m_window.GetHWND())
	{
		Diagnostics::Fatal(g_editorLogger, __FILE__, __LINE__, "UI::Implementation::InitializeWin32Backend: invalid window handle");
		return false;
	}

	ImGui_ImplWin32_Init(m_window.GetHWND());
	m_platformInitialized = true;
	return true;
}

void EditorImGuiContext::ApplyDpiScale(float dpiScale) noexcept
{
	ImGuiStyle& style = ImGui::GetStyle();
	style = ImGuiStyle{};
	ImGui::StyleColorsDark(&style);
	SparkleUiTheme::ApplyEditorialDarkTheme();
	style.FontScaleDpi = dpiScale;
	style.ScaleAllSizes(dpiScale);
}

bool EditorImGuiContext::IsReady() const noexcept
{
	return m_context != nullptr && m_platformInitialized;
}

void EditorImGuiContext::SubscribeToWindowEvents(ShortcutHandler shortcutHandler)
{
	m_shortcutHandler = std::move(shortcutHandler);
	auto handle = m_window.OnWindowMessage.Add([this](WindowMessageEvent& event) { HandleWindowMessage(event); });
	m_windowMessageHandle = ScopedEventHandle(m_window.OnWindowMessage, handle);
	auto dpiScaleHandle = m_window.OnDpiScaleChanged.Add([this](float dpiScale) { ApplyDpiScale(dpiScale); });
	m_windowDpiScaleHandle = ScopedEventHandle(m_window.OnDpiScaleChanged, dpiScaleHandle);
}

void EditorImGuiContext::UnsubscribeFromWindowEvents() noexcept
{
	m_windowDpiScaleHandle.Reset();
	m_windowMessageHandle.Reset();
	m_shortcutHandler = {};
}

void EditorImGuiContext::HandleWindowMessage(WindowMessageEvent& event)
{
	if (!m_platformInitialized)
	{
		return;
	}

	if (m_shortcutHandler && ImGui::GetCurrentContext() != nullptr && (event.lParam & (LPARAM{1} << 30)) == 0
		&& m_shortcutHandler(static_cast<std::uint32_t>(event.msg), static_cast<std::uintptr_t>(event.wParam), ImGui::GetIO().WantTextInput))
	{
		event.handled = true;
		return;
	}

	if (ImGui_ImplWin32_WndProcHandler(event.hWnd, event.msg, event.wParam, event.lParam) != 0)
	{
		event.handled = true;
	}
}

void EditorImGuiContext::BeginFrame(float deltaSeconds)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaSeconds;
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
