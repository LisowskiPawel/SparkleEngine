#include "PCH.h"
#include "Panels/MainMenuBarPanel.h"

#include "Level/Level.h"
#include "Level/LevelSession.h"
#include "Panels/TitleBarWindowControls.h"
#include "Util/UiUtil.h"
#include "Window/Window.h"

#include <imgui.h>
#include <algorithm>
#include <utility>

MainMenuBarPanel::MainMenuBarPanel(LevelSession* levelSession, Window* window) noexcept
{
	SetLevelSession(levelSession);
	SetWindow(window);
}

void MainMenuBarPanel::SetLevelSession(LevelSession* levelSession) noexcept
{
	m_levelSession = levelSession;
}

void MainMenuBarPanel::SetWindow(Window* window) noexcept
{
	m_window = window;
}

void MainMenuBarPanel::SetShaderToolsOpenHandler(std::function<void()> handler)
{
	m_shaderToolsOpenHandler = std::move(handler);
}

void MainMenuBarPanel::SetMeshToolsOpenHandler(std::function<void()> handler)
{
	m_meshToolsOpenHandler = std::move(handler);
}

void MainMenuBarPanel::SetTextureToolsOpenHandler(std::function<void()> handler)
{
	m_textureToolsOpenHandler = std::move(handler);
}

void MainMenuBarPanel::SetSettingsOpenHandler(std::function<void()> handler)
{
	m_settingsOpenHandler = std::move(handler);
}

void MainMenuBarPanel::SetViewportCaptureHandler(std::function<void()> handler)
{
	m_viewportCaptureHandler = std::move(handler);
}

void MainMenuBarPanel::SetConsoleOpenHandler(std::function<void()> handler)
{
	m_consoleOpenHandler = std::move(handler);
}

void MainMenuBarPanel::BuildOpenLevelMenu() noexcept
{
	if (m_levelSession == nullptr)
	{
		ImGui::MenuItem("No level manager", nullptr, false, false);
		return;
	}

	std::vector<std::string> levelNames = m_levelSession->GetRegisteredLevelNames();
	if (levelNames.empty())
	{
		ImGui::MenuItem("No levels found", nullptr, false, false);
		return;
	}

	const LevelAsset* activeLevel = m_levelSession->GetActiveLevel();
	const std::string activeLevelName = activeLevel != nullptr ? std::string(activeLevel->GetName()) : std::string();

	for (const std::string& levelName : levelNames)
	{
		const bool isActive = levelName == activeLevelName;
		const std::string levelLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Level, levelName.c_str());
		if (ImGui::MenuItem(levelLabel.c_str(), nullptr, isActive, !isActive))
		{
			m_levelSession->RequestLevelChange(levelName);
		}
	}
}

void MainMenuBarPanel::BuildFileMenu() noexcept
{
	const bool hasLevelSession = m_levelSession != nullptr;
	const bool hasActiveLevel = hasLevelSession && m_levelSession->HasActiveLevel();
	const bool levelChangeInProgress = hasLevelSession && m_levelSession->IsLevelChangeInProgress();

	const std::string openLevelLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::FolderOpen, "Open Level");
	if (ImGui::BeginMenu(openLevelLabel.c_str(), hasLevelSession && !levelChangeInProgress))
	{
		BuildOpenLevelMenu();
		ImGui::EndMenu();
	}

	const std::string saveAllLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Save, "Save All");
	if (ImGui::MenuItem(saveAllLabel.c_str(), nullptr, false, hasActiveLevel && !levelChangeInProgress))
	{
		m_levelSession->SaveActiveLevel();
	}

	const std::string captureLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Save, "Capture Viewport");
	if (ImGui::MenuItem(captureLabel.c_str(), nullptr, false, static_cast<bool>(m_viewportCaptureHandler)))
	{
		m_viewportCaptureHandler();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Quit", "Alt+F4", false, m_window != nullptr))
	{
		m_window->RequestClose();
	}
}

void MainMenuBarPanel::BuildWindowsMenu() noexcept
{
	const std::string consoleLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Console, "Console");
	if (ImGui::MenuItem(consoleLabel.c_str(), "~", false, static_cast<bool>(m_consoleOpenHandler)))
	{
		m_consoleOpenHandler();
	}

	const std::string settingsLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Settings, "Settings");
	if (ImGui::MenuItem(settingsLabel.c_str(), nullptr, false, static_cast<bool>(m_settingsOpenHandler)))
	{
		m_settingsOpenHandler();
	}

	ImGui::Separator();

	const std::string shadersLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Shader, "Shaders");
	if (ImGui::MenuItem(shadersLabel.c_str(), nullptr, false, static_cast<bool>(m_shaderToolsOpenHandler)))
	{
		m_shaderToolsOpenHandler();
	}

	const std::string meshesLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::StaticMesh, "Meshes");
	if (ImGui::MenuItem(meshesLabel.c_str(), nullptr, false, static_cast<bool>(m_meshToolsOpenHandler)))
	{
		m_meshToolsOpenHandler();
	}

	const std::string texturesLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Material, "Textures");
	if (ImGui::MenuItem(texturesLabel.c_str(), nullptr, false, static_cast<bool>(m_textureToolsOpenHandler)))
	{
		m_textureToolsOpenHandler();
	}
}

void MainMenuBarPanel::BuildUI() noexcept
{
	m_heightPixels = 0.0f;

	if (!ImGui::BeginMainMenuBar())
	{
		return;
	}

	m_heightPixels = ImGui::GetWindowSize().y;

	if (ImGui::BeginMenu("File"))
	{
		BuildFileMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window"))
	{
		BuildWindowsMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help"))
	{
		const std::string controlsLabel = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Help, "Controls");
		if (ImGui::MenuItem(controlsLabel.c_str()))
		{
			m_helpOpen = true;
		}
		ImGui::EndMenu();
	}
	DrawTitleBarWindowControls(m_window);

	ImGui::EndMainMenuBar();
}

void MainMenuBarPanel::BuildOverlays() noexcept
{
	BuildHelpWindow();
	BuildLevelStatus();
}

void MainMenuBarPanel::BuildHelpWindow() noexcept
{
	if (m_helpOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(520.0f, 320.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Editor Help", &m_helpOpen))
		{
			ImGui::TextWrapped(
			    "Start Editor or Game through Sparkle Launcher. File > Open Level selects a registered level; missing cooked content fails visibly. File > Save All explicitly saves the active level "
			    "source. Use the scene inspector for supported edits and undo/redo.");

			ImGui::TextWrapped(
			    "Hold right mouse over the viewport to look. W/A/S/D or arrows move, E/Space up, Q/C down, Shift accelerates, wheel changes speed. Esc releases the mouse. Window > Settings changes "
			    "rendering settings; Reset rendering defaults restores defaults. Quit or Alt+F4 closes Editor.");
		}
		ImGui::End();
	}
}

void MainMenuBarPanel::BuildLevelStatus() noexcept
{
	const bool failed = m_levelSession && !m_levelSession->GetLastLoadDiagnostic().empty();
	const bool focusFailure = failed && !m_levelFailureVisible;
	m_levelFailureVisible = failed;
	if (m_levelSession && (m_levelSession->IsLevelChangeInProgress() || failed))
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f, viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2((std::min) (560.0f * ImGui::GetStyle().FontScaleDpi, viewport->WorkSize.x), 0.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), viewport->WorkSize);
		if (focusFailure)
		{
			ImGui::SetNextWindowFocus();
		}

		if (ImGui::Begin("Level operation", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
		{
			ImGui::PushTextWrapPos(0.0f);
			if (m_levelSession->IsLevelChangeInProgress())
			{
				ImGui::TextUnformatted("Loading selected level. The previous level remains active until loading succeeds.");
			}
			ImGui::TextWrapped("%s", std::string(m_levelSession->GetLastLoadDiagnostic()).c_str());
			ImGui::TextUnformatted("Use File > Open Level to retry or select another level. Sync/cook missing content through Sparkle Launcher.");
			if (ImGui::Button("Quit Editor") && m_window)
			{
				m_window->RequestClose();
			}
			ImGui::PopTextWrapPos();
		}
		ImGui::End();
	}
}
