#include "PCH.h"

#include "UIImplementation.h"
#include "ImGui/EditorImGuiContext.h"

#include "EditorWorkspaceLayout.h"
#include "Console/EditorConsoleSystem.h"
#include "Input/InputSystem.h"
#include "Level/LevelSession.h"
#include "Level/Level.h"
#include "Workspace/MainMenuBarPanel.h"
#include "Scene/Inspection/SceneInspectorPanel.h"
#include "Scene/SceneOutlinerPanel.h"
#include "Settings/SettingsPanel.h"
#include "AssetInspection/UsedMeshesPanel.h"
#include "Shaders/ShaderToolsPanel.h"
#include "AssetInspection/UsedTexturesPanel.h"
#include "Viewport/ViewportPanel.h"
#include "Viewport/ViewportToolbar.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Timer.h"

#include <imgui.h>

#include <algorithm>

void UI::Implementation::NewFrame()
{
	const float deltaSeconds = static_cast<float>(m_timer->GetDelta(TimeDomain::Unscaled, TimeUnit::Seconds));
	m_imguiContext->BeginFrame(deltaSeconds);
}

void UI::Implementation::Build()
{
	if (m_transactionHistory && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_transactionHistory->FinishInteraction();
	}
	UpdateSceneModel();
	HandleTransactionShortcuts();
	const bool bDisableInteraction = m_levelSession != nullptr && m_levelSession->IsLevelChangeInProgress();
	BeginInputRouting(bDisableInteraction);
	const float mainMenuBarHeight = BuildMainMenuBar();
	BuildSceneOutliner(bDisableInteraction, mainMenuBarHeight);
	BuildCenterWorkspace(bDisableInteraction, mainMenuBarHeight);
	BuildSceneInspector(bDisableInteraction, mainMenuBarHeight);
	BuildUtilityPanels(bDisableInteraction);

#if USE_IMGUI_DEMO_WINDOW
	bool bShowDemoWindow = true;
	ImGui::ShowDemoWindow(&bShowDemoWindow);
#endif

	if (m_mainMenuBar)
	{
		m_mainMenuBar->BuildOverlays();
	}

	if (m_transactionHistory && !ImGui::IsAnyItemActive())
	{
		m_transactionHistory->FinishInteraction();
	}
	ImGui::Render();
}

void UI::Implementation::BeginInputRouting(bool bDisableInteraction)
{
	if (m_inputSystem == nullptr)
	{
		return;
	}

	const ImGuiIO& io = ImGui::GetIO();
	m_inputSystem->BeginInputRoutingFrame(bDisableInteraction, io.WantTextInput || io.WantCaptureKeyboard);
}

float UI::Implementation::BuildMainMenuBar()
{
	if (!m_mainMenuBar)
	{
		return 0.0f;
	}

	m_mainMenuBar->BuildUI();
	return m_mainMenuBar->GetHeight();
}

void UI::Implementation::BuildSceneOutliner(bool bDisableInteraction, float mainMenuBarHeight)
{
	if (!m_sceneOutlinerPanel)
	{
		return;
	}

	m_sceneOutlinerPanel->SetTopInset(mainMenuBarHeight);
	m_sceneOutlinerPanel->BuildUI(bDisableInteraction);
}

void UI::Implementation::BuildCenterWorkspace(bool bDisableInteraction, float mainMenuBarHeight)
{
	const ImGuiIO& io = ImGui::GetIO();
	const float outlinerWidth = m_sceneOutlinerPanel ? m_sceneOutlinerPanel->GetWidth() : EditorWorkspaceLayout::SceneOutlinerWidth;
	const float inspectorWidth = m_sceneInspectorPanel ? m_sceneInspectorPanel->GetWidth() : EditorWorkspaceLayout::SceneInspectorWidth;
	const float availableCenterHeight = (std::max) (0.0f, io.DisplaySize.y - mainMenuBarHeight);
	const float viewportWidth = (std::max) (EditorWorkspaceLayout::MinimumViewportExtent, io.DisplaySize.x - outlinerWidth - inspectorWidth);

	float viewportToolbarHeight = 0.0f;
	if (m_viewportToolbar)
	{
		m_viewportToolbar->SetGeometry(outlinerWidth, mainMenuBarHeight, viewportWidth);
		const LevelAsset* activeLevel = m_levelSession != nullptr ? m_levelSession->GetActiveLevel() : nullptr;
		m_viewportToolbar->Draw(activeLevel != nullptr ? activeLevel->GetName() : "<None>", bDisableInteraction);
		viewportToolbarHeight = m_viewportToolbar->GetHeightPixels();
	}

	const float availableViewportHeight = (std::max) (0.0f, availableCenterHeight - viewportToolbarHeight);
	const float consoleDockHeight = m_editorConsoleSystem ? m_editorConsoleSystem->GetDockHeight(availableViewportHeight) : 0.0f;
	BuildViewport(bDisableInteraction, mainMenuBarHeight + viewportToolbarHeight, consoleDockHeight, outlinerWidth, inspectorWidth);

	if (m_editorConsoleSystem)
	{
		m_editorConsoleSystem->BuildDockedUI(outlinerWidth, mainMenuBarHeight + availableCenterHeight, viewportWidth, availableViewportHeight, bDisableInteraction);
	}
}

void UI::Implementation::BuildViewport(bool bDisableInteraction, float topInset, float bottomInset, float outlinerWidth, float inspectorWidth)
{
	if (!m_viewportPanel)
	{
		return;
	}

	m_viewportPanel->SetTopInset(topInset);
	m_viewportPanel->SetBottomInset(bottomInset);
	m_viewportPanel->SetSideInsets(outlinerWidth, inspectorWidth);
	m_viewportPanel->BuildUI(*m_icons, bDisableInteraction);
	RegisterViewportInputRegion();
}

void UI::Implementation::RegisterViewportInputRegion()
{
	if (!m_viewportPanel || !m_inputSystem)
	{
		return;
	}

	float viewportLeft = 0.0f;
	float viewportTop = 0.0f;
	float viewportRight = 0.0f;
	float viewportBottom = 0.0f;
	if (!m_viewportPanel->GetInputBounds(viewportLeft, viewportTop, viewportRight, viewportBottom))
	{
		return;
	}

	m_inputSystem->RegisterInputTargetRegion(viewportLeft, viewportTop, viewportRight, viewportBottom, m_viewportPanel->GetTargetInputLayer());
}

void UI::Implementation::BuildSceneInspector(bool bDisableInteraction, float mainMenuBarHeight)
{
	if (!m_sceneInspectorPanel)
	{
		return;
	}

	m_sceneInspectorPanel->SetTopInset(mainMenuBarHeight);
	m_sceneInspectorPanel->BuildUI(bDisableInteraction);
}

void UI::Implementation::BuildUtilityPanels(bool bDisableInteraction)
{
	if (m_shaderToolsPanel)
	{
		m_shaderToolsPanel->BuildUI(bDisableInteraction);
	}
	if (m_usedMeshesPanel)
	{
		m_usedMeshesPanel->BuildUI(bDisableInteraction);
	}
	if (m_usedTexturesPanel)
	{
		m_usedTexturesPanel->BuildUI(bDisableInteraction);
	}
	if (m_settingsPanel)
	{
		m_settingsPanel->BuildUI(bDisableInteraction);
	}
}
