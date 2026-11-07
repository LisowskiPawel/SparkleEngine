#include "PCH.h"
#include "Workspace/MainMenuBarPanel.h"

#include "Level/Level.h"
#include "Level/LevelSession.h"
#include "Workspace/TitleBarWindowControls.h"
#include "Icons/EditorIcon.h"
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
		const bool bIsActive = levelName == activeLevelName;
		const std::string levelLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Level, levelName.c_str());
		if (ImGui::MenuItem(levelLabel.c_str(), nullptr, bIsActive, !bIsActive))
		{
			m_levelSession->RequestLevelChange(levelName);
		}
	}
}

void MainMenuBarPanel::BuildFileMenu() noexcept
{
	const bool bHasLevelSession = m_levelSession != nullptr;
	const bool bHasActiveLevel = bHasLevelSession && m_levelSession->HasActiveLevel();
	const bool bLevelChangeInProgress = bHasLevelSession && m_levelSession->IsLevelChangeInProgress();

	const std::string openLevelLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::FolderOpen, "Open Level");
	if (ImGui::BeginMenu(openLevelLabel.c_str(), bHasLevelSession && !bLevelChangeInProgress))
	{
		BuildOpenLevelMenu();
		ImGui::EndMenu();
	}

	const std::string saveAllLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Save, "Save All");
	if (ImGui::MenuItem(saveAllLabel.c_str(), nullptr, false, bHasActiveLevel && !bLevelChangeInProgress))
	{
		m_levelSession->SaveActiveLevel();
	}

	const std::string captureLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Save, "Capture Viewport");
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
	const std::string consoleLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Console, "Console");
	if (ImGui::MenuItem(consoleLabel.c_str(), "~", false, static_cast<bool>(m_consoleOpenHandler)))
	{
		m_consoleOpenHandler();
	}

	const std::string settingsLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Settings, "Settings");
	if (ImGui::MenuItem(settingsLabel.c_str(), nullptr, false, static_cast<bool>(m_settingsOpenHandler)))
	{
		m_settingsOpenHandler();
	}

	ImGui::Separator();

	const std::string shadersLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Shader, "Shaders");
	if (ImGui::MenuItem(shadersLabel.c_str(), nullptr, false, static_cast<bool>(m_shaderToolsOpenHandler)))
	{
		m_shaderToolsOpenHandler();
	}

	const std::string meshesLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::StaticMesh, "Meshes");
	if (ImGui::MenuItem(meshesLabel.c_str(), nullptr, false, static_cast<bool>(m_meshToolsOpenHandler)))
	{
		m_meshToolsOpenHandler();
	}

	const std::string texturesLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Material, "Textures");
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
		const std::string controlsLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Help, "Controls");
		if (ImGui::MenuItem(controlsLabel.c_str()))
		{
			m_bHelpOpen = true;
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
	if (m_bHelpOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(520.0f, 320.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Editor Help", &m_bHelpOpen))
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
	const bool bFailed = m_levelSession && !m_levelSession->GetLastLoadDiagnostic().empty();
	const bool bFocusFailure = bFailed && !m_bLevelFailureVisible;
	m_bLevelFailureVisible = bFailed;
	if (m_levelSession && (m_levelSession->IsLevelChangeInProgress() || bFailed))
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f, viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2((std::min) (560.0f * ImGui::GetStyle().FontScaleDpi, viewport->WorkSize.x), 0.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), viewport->WorkSize);
		if (bFocusFailure)
		{
			ImGui::SetNextWindowFocus();
		}

		const bool bOperationWindowOpen = ImGui::Begin("Level operation", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		
		if (bOperationWindowOpen)
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
