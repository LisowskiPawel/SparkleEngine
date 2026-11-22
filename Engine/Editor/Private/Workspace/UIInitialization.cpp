#include "PCH.h"

#include "UIImplementation.h"
#include "ImGui/EditorImGuiContext.h"
#include "Editor/Public/Icons/EditorIconService.h"

#include "EditorWorkspaceLayout.h"
#include "Console/EditorConsoleSystem.h"
#include "Console/ConsoleCommands.h"
#include "Workspace/MainMenuBarPanel.h"
#include "Scene/Inspection/SceneInspectorPanel.h"
#include "Scene/SceneOutlinerPanel.h"
#include "Settings/SettingsPanel.h"
#include "AssetInspection/UsedMeshesPanel.h"
#include "Shaders/ShaderToolsPanel.h"
#include "AssetInspection/UsedTexturesPanel.h"
#include "Viewport/ViewportPanel.h"
#include "Viewport/ViewportToolbar.h"
#include "Settings/EngineRenderingSettingsController.h"
#include "ImGui/ImGuiRenderPacketBuilder.h"
#include "Style/SparkleUiTheme.h"
#include "Viewport/EditorViewportSession.h"

#include <memory>
#include <utility>

void UI::Implementation::InitializeRenderingSettings(EditorHostServices& hostServices)
{
	m_renderingSettings = std::make_unique<EngineRenderingSettingsController>(
	    std::move(hostServices.RenderingSettings),
	    std::move(hostServices.SubmitRenderingSettings),
	    std::move(hostServices.CaptureRenderingSettings));
}

void UI::Implementation::InitializeConsole(CVarControlExecutor&& consoleVariables)
{
	m_consoleVariables = std::move(consoleVariables);

	m_editorConsoleSystem = std::make_unique<EditorConsoleSystem>(ConsoleCommandActions{
	    .Variables = m_consoleVariables,
	    .ReloadShaders = [this]()
	    {
		    m_bShaderReloadRequested = true;
		    return ConsoleCommandResult::Success("Shader reload requested.");
	    }});
}

void UI::Implementation::InitializeWorkspace(std::function<void()> requestRestart)
{
	InitializeCorePanels(std::move(requestRestart));
	InitializeViewport();
	InitializeAssetPanels();
	InitializeScenePanels();
	ConfigureMainMenuBarWindowActions();
}

void UI::Implementation::InitializeCorePanels(std::function<void()> requestRestart)
{
	m_mainMenuBar = std::make_unique<MainMenuBarPanel>(m_levelSession, &m_window);
	m_settingsPanel = std::make_unique<SettingsPanel>();
	m_settingsPanel->SetRenderingSettings(m_renderingSettings.get());

	m_settingsPanel->SetRestartHandler(std::move(requestRestart));
}

void UI::Implementation::InitializeViewport()
{
	m_viewportSession = std::make_unique<EditorViewportSession>();
	m_viewportToolbar = std::make_unique<ViewportToolbar>(*m_viewportSession, m_renderingSettings->GetState(), m_consoleVariables);
	m_viewportPanel = std::make_unique<ViewportPanel>(EditorWorkspaceLayout::SceneOutlinerWidth, EditorWorkspaceLayout::SceneInspectorWidth);
	m_viewportPanel->SetExposureOverrides(m_viewportSession->GetSettings().Exposure);
}

void UI::Implementation::InitializeAssetPanels()
{
	m_shaderToolsPanel = std::make_unique<ShaderToolsPanel>();
	m_usedMeshesPanel = std::make_unique<UsedMeshesPanel>();
	m_usedTexturesPanel = std::make_unique<UsedTexturesPanel>();
	m_shaderToolsPanel->SetReloadHandler([this]() { m_bShaderReloadRequested = true; });
}

void UI::Implementation::InitializeScenePanels()
{
	m_sceneOutlinerPanel = std::make_unique<SceneOutlinerPanel>(m_scene, EditorWorkspaceLayout::SceneOutlinerWidth);
	m_sceneInspectorPanel = std::make_unique<SceneInspectorPanel>(m_scene, EditorWorkspaceLayout::SceneInspectorWidth);
}

void UI::Implementation::ConfigureMainMenuBarWindowActions()
{
	m_mainMenuBar->SetShaderToolsOpenHandler([this]() { m_shaderToolsPanel->SetOpen(true); });
	m_mainMenuBar->SetTextureToolsOpenHandler([this]() { m_usedTexturesPanel->SetOpen(true); });
	m_mainMenuBar->SetMeshToolsOpenHandler([this]() { m_usedMeshesPanel->SetOpen(true); });
	m_mainMenuBar->SetSettingsOpenHandler([this]() { m_settingsPanel->SetOpen(true); });
	m_mainMenuBar->SetViewportCaptureHandler([this]() { m_viewportPanel->RequestOutputAction(ViewportOutputAction::CapturePresentation); });
	m_mainMenuBar->SetConsoleOpenHandler([this]() { m_editorConsoleSystem->OpenConsole(); });
}

void UI::Implementation::InitializeImGuiContext()
{
	m_imguiContext = std::make_unique<EditorImGuiContext>(m_window, m_inputSystem);
	m_imguiContext->Initialize();
	ImGuiRenderPacketBuilder::ConfigureProducerContext();
	SparkleUiTheme::ConfigureTypography();
	m_icons = std::make_unique<EditorIconService>();
}

void UI::Implementation::SubscribeToWindowEvents()
{
	m_imguiContext->SubscribeToWindowEvents([this](Key key) { return m_editorConsoleSystem->HandleShortcut(key); });
}
