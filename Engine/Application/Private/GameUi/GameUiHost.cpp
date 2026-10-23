#include "PCH.h"
#include "GameUi/GameUiHost.h"

#include "Application.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Level/Level.h"
#include "Level/LevelSession.h"
#include "Renderer/Public/UI/ImGuiRenderPacketBuilder.h"
#include "Time/Timer.h"
#include "Window/Window.h"
#if !SPARKLE_BUILD_SHIPPING
  #include "GameConsole/GameConsoleOverlay.h"
#endif

#include <backends/imgui_impl_win32.h>
#include <imgui.h>
#include <algorithm>
#include <utility>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GameUiHost::GameUiHost(Timer& timer, Window& window, LevelSession& levels, std::function<void()> releaseMouse, CVarControlExecutor consoleExecutor) :
    m_timer(timer),
    m_window(window),
    m_levels(levels),
    m_releaseMouse(std::move(releaseMouse))
{
	try
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		m_contextReady = true;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::GetIO().IniFilename = nullptr;
		ImGuiRenderPacketBuilder::ConfigureProducerContext();

		auto applyDpi = [](float scale)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style = ImGuiStyle{};
			ImGui::StyleColorsDark(&style);
			style.FontScaleDpi = scale;
			style.ScaleAllSizes(scale);
		};

		applyDpi(window.GetDpiScale());
		if (!ImGui_ImplWin32_Init(window.GetHWND()))
		{
			throw Diagnostics::Error("Sparkle Game UI could not initialize window input.");
		}
		m_backendReady = true;
		m_packetBuilder = std::make_unique<ImGuiRenderPacketBuilder>();
#if !SPARKLE_BUILD_SHIPPING
		if (consoleExecutor)
		{
			m_console = std::make_unique<GameConsoleOverlay>(std::move(consoleExecutor));
		}
#else
		(void) consoleExecutor;
#endif
		m_windowMessageHandle = ScopedEventHandle(window.OnWindowMessage, window.OnWindowMessage.Add([this](WindowMessageEvent& event) { HandleWindowMessage(event); }));
		m_dpiHandle = ScopedEventHandle(window.OnDpiScaleChanged, window.OnDpiScaleChanged.Add(applyDpi));
		m_levelFailureHandle = ScopedEventHandle(levels.GetLevelChangeEvents().OnLevelLoadFailed, levels.GetLevelChangeEvents().OnLevelLoadFailed.Add([this](std::string_view) { OpenMenu(); }));
		OpenMenu();
	}
	catch (...)
	{
		Shutdown();
		throw;
	}
}

GameUiHost::~GameUiHost() noexcept
{
	Shutdown();
}

void GameUiHost::Shutdown() noexcept
{
	m_levelFailureHandle.Reset();
	m_dpiHandle.Reset();
	m_windowMessageHandle.Reset();
#if !SPARKLE_BUILD_SHIPPING
	m_console.reset();
#endif
	m_packetBuilder.reset();
	if (m_backendReady)
	{
		ImGui_ImplWin32_Shutdown();
		m_backendReady = false;
	}
	if (m_contextReady)
	{
		ImGui::DestroyContext();
		m_contextReady = false;
	}
}

void GameUiHost::OpenMenu()
{
	m_releaseMouse();
	m_menuOpen = true;
}

void GameUiHost::HandleWindowMessage(WindowMessageEvent& event)
{
	if (event.msg == WM_KEYDOWN && (event.lParam & (1 << 30)) == 0)
	{
		if (event.wParam == VK_ESCAPE || event.wParam == VK_F1)
		{
			if (m_menuOpen && event.wParam == VK_ESCAPE)
			{
				m_menuOpen = false;
			}
			else
			{
				OpenMenu();
			}
			event.handled = true;
		}
#if !SPARKLE_BUILD_SHIPPING
		if (event.wParam == VK_OEM_3 && m_console)
		{
			m_releaseMouse();
			m_console->ToggleVisibility();
			event.handled = true;
		}
#endif
	}
	// Always deliver focus and button releases to ImGui, including when the menu is closed.
	if (ImGui_ImplWin32_WndProcHandler(event.hWnd, event.msg, event.wParam, event.lParam) != 0)
	{
		event.handled = true;
	}
}

UiRenderPacket GameUiHost::Update()
{
	ImGui::GetIO().DeltaTime = (std::max) (0.000001f, static_cast<float>(m_timer.GetDelta(TimeDomain::Unscaled, TimeUnit::Seconds)));
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (m_menuOpen || m_levels.IsLevelChangeInProgress())
	{
		DrawMenu();
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_Always);
		ImGui::Begin("##GameMenuHint", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::Button("Menu / Help (Esc or F1)"))
		{
			OpenMenu();
		}
		ImGui::End();
	}
#if !SPARKLE_BUILD_SHIPPING
	if (m_console)
	{
		m_console->Update();
	}
#endif
	ImGui::Render();
	return m_packetBuilder->Build(*ImGui::GetDrawData(), UiPresentationMode::HostOverlay);
}

void GameUiHost::DrawMenu()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const float scale = m_window.GetDpiScale();
	ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2((std::min) (620.0f * scale, viewport->WorkSize.x), (std::min) (720.0f * scale, viewport->WorkSize.y)), ImGuiCond_Always);
	if (!ImGui::Begin("Sparkle Game"))
	{
		ImGui::End();
		return;
	}
	const LevelAsset* active = m_levels.GetActiveLevel();
	ImGui::Text("Active level: %s", active ? std::string(active->GetName()).c_str() : "none");
	if (m_levels.IsLevelChangeInProgress())
	{
		ImGui::TextUnformatted("Loading selected level. The previous level stays active until loading succeeds.");
		ImGui::TextUnformatted("You can quit while loading.");
	}
	if (!m_levels.GetLastLoadDiagnostic().empty())
	{
		ImGui::TextWrapped("Level operation failed: %s", std::string(m_levels.GetLastLoadDiagnostic()).c_str());
		ImGui::TextWrapped("Select another level below, or close Game and sync/cook the requested level in Sparkle Launcher.");
	}
	if (ImGui::Button("Resume"))
	{
		m_menuOpen = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Quit"))
	{
		m_window.RequestClose();
	}
	DrawLevelSelection();
	DrawControls();
	ImGui::TextWrapped("Rendering settings are configured in Sparkle Editor.");
	ImGui::End();
}

bool GameUiHost::WantsInputCapture() const noexcept
{
	if (m_menuOpen || m_levels.IsLevelChangeInProgress())
	{
		return true;
	}
#if !SPARKLE_BUILD_SHIPPING
	if (m_console && m_console->IsVisible())
	{
		return true;
	}
#endif
	return ImGui::GetIO().WantCaptureMouse;
}

void GameUiHost::DrawLevelSelection()
{
	const LevelAsset* active = m_levels.GetActiveLevel();
	if (ImGui::CollapsingHeader("Select level", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextWrapped("Use Sparkle Launcher to sync and cook additional levels.");
		for (const std::string& name : m_levels.GetRegisteredLevelNames())
		{
			if (ImGui::Selectable(name.c_str(), active && active->GetName() == name))
			{
				m_levels.RequestLevelChange(name);
			}
		}
	}
}

void GameUiHost::DrawControls()
{
	const LevelAsset* active = m_levels.GetActiveLevel();
	if (ImGui::CollapsingHeader("Controls / Help", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextWrapped(
		    "Hold right mouse to look. W/A/S/D or arrows move; E/Space moves up, Q/C down. Shift accelerates; mouse wheel changes speed. Esc releases mouse and opens this menu. F1 opens help. Alt+F4 "
		    "quits.");

		ImGui::TextWrapped("Reset camera reloads the active level. No source file is saved by Game.");
		if (ImGui::Button("Reset camera / reload level") && active)
		{
			m_levels.RequestLevelReload();
		}
	}
}
