#include "PCH.h"

#include "UIImplementation.h"
#include "ImGui/EditorImGuiContext.h"

#include "EditorWorkspaceLayout.h"
#include "Console/EditorConsoleSystem.h"
#include "Input/InputSystem.h"
#include "Level/LevelSession.h"
#include "Workspace/MainMenuBarPanel.h"
#include "Scene/Inspection/SceneInspectorPanel.h"
#include "Scene/SceneOutlinerPanel.h"
#include "Settings/SettingsPanel.h"
#include "AssetInspection/UsedMeshesPanel.h"
#include "Shaders/ShaderToolsPanel.h"
#include "AssetInspection/UsedTexturesPanel.h"
#include "Viewport/ViewportPanel.h"
#include "Viewport/ViewportToolbar.h"
#include "Timer.h"

#include <imgui.h>

#include <algorithm>
#include <string_view>

void UI::Implementation::NewFrame()
{
	const float deltaSeconds = static_cast<float>(m_timer.GetDelta(TimeDomain::Unscaled, TimeUnit::Seconds));
	m_imguiContext->BeginFrame(deltaSeconds);
}

void UI::Implementation::Build()
{
	EditorTransactionHistory& history = m_scene.GetHistory();
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		history.FinishInteraction();
	}
	m_scene.Refresh();
	const bool bDisableInteraction = m_levelSession != nullptr && m_levelSession->IsLevelChangeInProgress();
	HandleTransactionShortcuts(bDisableInteraction);
	BeginInputRouting(bDisableInteraction);
	const float mainMenuBarHeight = BuildMainMenuBar();
	BuildSceneOutliner(bDisableInteraction, mainMenuBarHeight);
	BuildCenterWorkspace(bDisableInteraction, mainMenuBarHeight);
	BuildSceneInspector(bDisableInteraction, mainMenuBarHeight);
	BuildUtilityPanels(bDisableInteraction);

	m_mainMenuBar->BuildOverlays();

	if (!ImGui::IsAnyItemActive())
	{
		history.FinishInteraction();
	}
	ImGui::Render();
}

void UI::Implementation::HandleTransactionShortcuts(bool bDisableInteraction)
{
	if (bDisableInteraction || ImGui::GetIO().WantTextInput)
	{
		return;
	}

	const EditorSceneModel* model = m_scene.GetModel();
	if (model == nullptr)
	{
		return;
	}

	EditorTransactionHistory& history = m_scene.GetHistory();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, ImGuiInputFlags_RouteGlobal))
	{
		(void) history.Undo(model->GetWorldGeneration());
	}
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Y, ImGuiInputFlags_RouteGlobal))
	{
		(void) history.Redo(model->GetWorldGeneration());
	}
}

void UI::Implementation::BeginInputRouting(bool bDisableInteraction)
{
	const ImGuiIO& io = ImGui::GetIO();
	m_inputSystem.BeginInputRoutingFrame(bDisableInteraction, io.WantTextInput || io.WantCaptureKeyboard);
}

float UI::Implementation::BuildMainMenuBar()
{
	m_mainMenuBar->BuildUI();
	return m_mainMenuBar->GetHeight();
}

void UI::Implementation::BuildSceneOutliner(bool bDisableInteraction, float mainMenuBarHeight)
{
	m_sceneOutlinerPanel->SetTopInset(mainMenuBarHeight);
	m_sceneOutlinerPanel->BuildUI(bDisableInteraction);
}

void UI::Implementation::BuildCenterWorkspace(bool bDisableInteraction, float mainMenuBarHeight)
{
	const ImGuiIO& io = ImGui::GetIO();
	const float outlinerWidth = m_sceneOutlinerPanel->GetWidth();
	const float inspectorWidth = m_sceneInspectorPanel->GetWidth();
	const float availableCenterHeight = (std::max) (0.0f, io.DisplaySize.y - mainMenuBarHeight);
	const float viewportWidth = (std::max) (EditorWorkspaceLayout::MinimumViewportExtent, io.DisplaySize.x - outlinerWidth - inspectorWidth);

	m_viewportToolbar->SetGeometry(outlinerWidth, mainMenuBarHeight, viewportWidth);
	const std::string_view activeLevelName = m_levelSession != nullptr ? m_levelSession->GetActiveLevelName() : std::string_view{};
	const std::string_view displayLevelName = activeLevelName.empty() ? "<None>" : activeLevelName;
	const RenderViewMode viewMode = GetViewportRenderRequest().ViewMode;
	const auto selectedMode = m_viewportToolbar->Draw(displayLevelName, viewMode, bDisableInteraction);
	if (selectedMode)
	{
		m_viewportPanel->SetViewMode(*selectedMode);
	}
	const float viewportToolbarHeight = m_viewportToolbar->GetHeightPixels();

	const float availableViewportHeight = (std::max) (0.0f, availableCenterHeight - viewportToolbarHeight);
	const float consoleDockHeight = m_editorConsoleSystem->GetDockHeight(availableViewportHeight);
	BuildViewport(bDisableInteraction, mainMenuBarHeight + viewportToolbarHeight, consoleDockHeight, outlinerWidth, inspectorWidth);

	m_editorConsoleSystem->BuildDockedUI(outlinerWidth, mainMenuBarHeight + availableCenterHeight, viewportWidth, availableViewportHeight, bDisableInteraction);
}

void UI::Implementation::BuildViewport(bool bDisableInteraction, float topInset, float bottomInset, float outlinerWidth, float inspectorWidth)
{
	m_viewportPanel->SetTopInset(topInset);
	m_viewportPanel->SetBottomInset(bottomInset);
	m_viewportPanel->SetSideInsets(outlinerWidth, inspectorWidth);
	m_viewportPanel->BuildUI(*m_icons, bDisableInteraction);
	RegisterViewportInputRegion();
}

void UI::Implementation::RegisterViewportInputRegion()
{
	float viewportLeft = 0.0f;
	float viewportTop = 0.0f;
	float viewportRight = 0.0f;
	float viewportBottom = 0.0f;
	if (!m_viewportPanel->GetInputBounds(viewportLeft, viewportTop, viewportRight, viewportBottom))
	{
		return;
	}

	m_inputSystem.RegisterInputTargetRegion(viewportLeft, viewportTop, viewportRight, viewportBottom, m_viewportPanel->GetTargetInputLayer());
}

void UI::Implementation::BuildSceneInspector(bool bDisableInteraction, float mainMenuBarHeight)
{
	m_sceneInspectorPanel->SetTopInset(mainMenuBarHeight);
	m_sceneInspectorPanel->BuildUI(bDisableInteraction);
}

void UI::Implementation::BuildUtilityPanels(bool bDisableInteraction)
{
	m_shaderToolsPanel->BuildUI(bDisableInteraction);
	m_usedMeshesPanel->BuildUI(bDisableInteraction);
	m_usedTexturesPanel->BuildUI(bDisableInteraction);
	m_settingsPanel->BuildUI(bDisableInteraction);
}
