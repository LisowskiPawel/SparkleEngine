#include "PCH.h"
#include "ImGui/EditorImGuiContext.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Style/SparkleUiTheme.h"
#include "Window/Window.h"
#include "Window/SdlWindowInterop.h"
#include "Input/InputSystem.h"

#include <backends/imgui_impl_sdl3.h>
#include <SDL3/SDL.h>
#include <imgui.h>

#include <utility>

EditorImGuiContext::EditorImGuiContext(Window& window, InputSystem& inputSystem) noexcept :
    m_window(window),
    m_inputSystem(inputSystem)
{
}

EditorImGuiContext::~EditorImGuiContext() noexcept
{
	UnsubscribeFromWindowEvents();
	if (m_bPlatformInitialized)
	{
		ImGui_ImplSDL3_Shutdown();
	}
	if (m_context != nullptr)
	{
		ImGui::DestroyContext(m_context);
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
	m_bPlatformInitialized = ImGui_ImplSDL3_InitForOther(GetSdlWindow(m_window));
	if (m_bPlatformInitialized)
	{
		ImGui_ImplSDL3_SetGamepadMode(ImGui_ImplSDL3_GamepadMode_Manual, nullptr, 0);
	}
	return m_bPlatformInitialized;
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

void EditorImGuiContext::SubscribeToWindowEvents(ShortcutHandler shortcutHandler)
{
	m_shortcutHandler = std::move(shortcutHandler);
	auto& events = GetSdlWindowEvents(m_window);
	const auto eventHandle = events.Add([this](const SdlWindowEvent& event) { HandleWindowEvent(event); });
	m_windowEventHandle = ScopedEventHandle(events, eventHandle);
	const auto dpiHandle = m_window.OnDpiScaleChanged.Add([this](float scale) { ApplyDpiScale(scale); });
	m_windowDpiScaleHandle = ScopedEventHandle(m_window.OnDpiScaleChanged, dpiHandle);
	const auto cancelHandle = m_window.OnInputCanceled.Add([this](InputCancellation cancellation) { CancelInput(cancellation); });
	m_windowCancellationHandle = ScopedEventHandle(m_window.OnInputCanceled, cancelHandle);
}

void EditorImGuiContext::UnsubscribeFromWindowEvents() noexcept
{
	m_windowCancellationHandle.Reset();
	m_windowDpiScaleHandle.Reset();
	m_windowEventHandle.Reset();
	m_shortcutHandler = {};
}

void EditorImGuiContext::CancelInput(InputCancellation cancellation) noexcept
{
	if (!m_bPlatformInitialized)
	{
		return;
	}
	// Clear the stock backend's drag bookkeeping through its public event contract.
	// These UI releases do not pass through Core and cannot duplicate logical routed releases.
	SDL_Event release{};
	release.type = SDL_EVENT_MOUSE_BUTTON_UP;
	release.button.windowID = SDL_GetWindowID(GetSdlWindow(m_window));
	for (Uint8 button = SDL_BUTTON_LEFT; button <= SDL_BUTTON_X2; ++button)
	{
		release.button.button = button;
		ImGui_ImplSDL3_ProcessEvent(&release);
	}
	if (cancellation == InputCancellation::All)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddFocusEvent(false);
		if (SDL_GetKeyboardFocus() == GetSdlWindow(m_window))
		{
			io.AddFocusEvent(true);
		}
	}
	m_bSuppressShortcutText = false;
}

void EditorImGuiContext::HandleWindowEvent(const SdlWindowEvent& message)
{
	if (!m_bPlatformInitialized || !IsSdlWindowEventCurrent(m_window, message))
	{
		return;
	}
	const SDL_Event& event = message.Value;
	if (event.type == SDL_EVENT_KEY_DOWN)
	{
		m_bSuppressShortcutText = false;
		const bool bShortcutKey = event.key.scancode == SDL_SCANCODE_GRAVE && !event.key.repeat;
		const bool bAcceptShortcut = !ImGui::GetIO().WantTextInput && static_cast<bool>(m_shortcutHandler);
		if (bShortcutKey && bAcceptShortcut)
		{
			m_bSuppressShortcutText = m_shortcutHandler(Key::Grave);
			if (m_bSuppressShortcutText)
			{
				return;
			}
		}
	}
	if (event.type == SDL_EVENT_TEXT_INPUT && m_bSuppressShortcutText)
	{
		m_bSuppressShortcutText = false;
		const std::string_view text(event.text.text);
		if (text == "`" || text == "~")
		{
			return;
		}
	}
	ImGui_ImplSDL3_ProcessEvent(&event);
}

void EditorImGuiContext::BeginFrame(float deltaSeconds)
{
	ImGuiIO& io = ImGui::GetIO();
	if (m_inputSystem.IsMouseCaptured())
	{
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	}
	else
	{
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
	}
	m_bSuppressShortcutText = false;
	io.DeltaTime = deltaSeconds;
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}
