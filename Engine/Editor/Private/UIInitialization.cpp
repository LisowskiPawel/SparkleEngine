#include "PCH.h"

#include "UIImplementation.h"
#include "ImGui/EditorImGuiContext.h"
#include "Editor/Public/Icons/EditorIconService.h"

#include "EditorWorkspaceLayout.h"
#include "Console/EditorConsoleSystem.h"
#include "Panels/MainMenuBarPanel.h"
#include "Panels/SceneInspectorPanel.h"
#include "Panels/SceneOutlinerPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/UsedMeshesPanel.h"
#include "Panels/UsedShadersPanel.h"
#include "Panels/UsedTexturesPanel.h"
#include "Panels/ViewportPanel.h"
#include "Viewport/ViewportToolbar.h"
#include "Settings/EngineRenderingSettingsController.h"
#include "Renderer/Public/UI/ImGuiRenderPacketBuilder.h"
#include "Scene/Model/EditorSceneModel.h"
#include "Scene/Model/EditorSceneModelBuilder.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Settings/EditorRestartService.h"
#include "Style/SparkleUiTheme.h"
#include "Viewport/EditorViewportSession.h"

#include <memory>
#include <string>
#include <utility>

void UI::Implementation::InitializeSceneEditing(EditorHostServices& hostServices)
{
	m_sceneModelBuilder = std::make_unique<EditorSceneModelBuilder>(EditorSceneSource{
	    .AcquireReadView = std::move(hostServices.AcquireWorldReadView),
	    .ReadChanges = std::move(hostServices.ReadWorldChanges),
	    .AcknowledgeChanges = std::move(hostServices.AcknowledgeWorldChanges),
	    .WorldGeneration = std::move(hostServices.WorldGeneration),
	    .MaterialVariants = std::move(hostServices.MaterialVariants)});

	m_transactionHistory = std::make_unique<EditorTransactionHistory>(std::move(hostServices.SubmitWorldEdit));
}

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
	m_editorConsoleSystem = std::make_unique<EditorConsoleSystem>(m_consoleVariables);
}

void UI::Implementation::InitializeWorkspace()
{
	InitializeCorePanels();
	InitializeViewport();
	InitializeAssetPanels();
	InitializeScenePanels();
}

void UI::Implementation::InitializeCorePanels()
{
	m_mainMenuBar = std::make_unique<MainMenuBarPanel>(m_levelSession, m_window);
	ConfigureMainMenuBarWindowActions();
	m_restartService = std::make_unique<EditorRestartService>();
	m_settingsPanel = std::make_unique<SettingsPanel>();
	m_settingsPanel->SetRenderingSettings(m_renderingSettings.get());

	m_settingsPanel->SetRestartHandler(
	    [this]()
	    {
		    if (m_window != nullptr && m_restartService != nullptr)
		    {
			    (void) m_restartService->Restart(*m_window);
		    }
	    });
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
	m_usedShadersPanel = std::make_unique<UsedShadersPanel>();
	m_usedShadersPanel->SetGenerationProvider(m_shaderGenerationProvider);
	m_usedMeshesPanel = std::make_unique<UsedMeshesPanel>();
	m_usedMeshesPanel->SetDiagnosticsProvider(m_meshDiagnosticsProvider);
	m_usedMeshesPanel->SetPreviewGeometryProvider(m_meshPreviewProvider);
	m_usedTexturesPanel = std::make_unique<UsedTexturesPanel>();
	m_usedTexturesPanel->SetDiagnosticsProvider(m_textureDiagnosticsProvider);
	m_usedShadersPanel->SetReloadHandler([this]() { m_shaderReloadRequested = true; });
	m_usedShadersPanel->SetRecookAllHandler([this]() { m_shaderRecookRequested = true; });

	m_usedShadersPanel->SetRecookHandler(
	    [this](const std::string& shaderId)
	    {
		    if (m_editorConsoleSystem)
		    {
			    m_editorConsoleSystem->SubmitLine("RecompileShaders Shader " + shaderId);
		    }
	    });
}

void UI::Implementation::InitializeScenePanels()
{
	m_sceneModel = m_sceneModelBuilder->Update();
	if (m_sceneModel && !m_sceneModel->GetCameras().empty())
	{
		m_sceneSelection = SceneObjectSelection::Camera(m_sceneModel->GetCameras().front().Entity);
	}

	m_sceneOutlinerPanel = std::make_unique<SceneOutlinerPanel>(m_sceneSelection, *m_transactionHistory, EditorWorkspaceLayout::SceneOutlinerWidth);
	m_sceneInspectorPanel = std::make_unique<SceneInspectorPanel>(m_sceneSelection, *m_transactionHistory, EditorWorkspaceLayout::SceneInspectorWidth);
}

void UI::Implementation::ConfigureMainMenuBarWindowActions()
{
	if (!m_mainMenuBar)
	{
		return;
	}

	m_mainMenuBar->SetShaderToolsOpenHandler(
	    [this]()
	    {
		    if (m_usedShadersPanel)
		    {
			    m_usedShadersPanel->SetOpen(true);
		    }
	    });

	m_mainMenuBar->SetTextureToolsOpenHandler(
	    [this]()
	    {
		    if (m_usedTexturesPanel)
		    {
			    m_usedTexturesPanel->SetOpen(true);
		    }
	    });

	m_mainMenuBar->SetMeshToolsOpenHandler(
	    [this]()
	    {
		    if (m_usedMeshesPanel)
		    {
			    m_usedMeshesPanel->SetOpen(true);
		    }
	    });

	m_mainMenuBar->SetSettingsOpenHandler(
	    [this]()
	    {
		    if (m_settingsPanel)
		    {
			    m_settingsPanel->SetOpen(true);
		    }
	    });

	m_mainMenuBar->SetViewportCaptureHandler(
	    [this]()
	    {
		    if (m_viewportPanel)
		    {
			    m_viewportPanel->RequestOutputAction(ViewportOutputAction::CapturePresentation);
		    }
	    });

	m_mainMenuBar->SetConsoleOpenHandler(
	    [this]()
	    {
		    if (m_editorConsoleSystem)
		    {
			    m_editorConsoleSystem->OpenConsole();
		    }
	    });
}

void UI::Implementation::InitializeImGuiContext()
{
	m_imguiContext = std::make_unique<EditorImGuiContext>(*m_window);
	m_imguiContext->Initialize();
	ImGuiRenderPacketBuilder::ConfigureProducerContext();
	SparkleUiTheme::ConfigureTypography();
	m_icons = std::make_unique<EditorIconService>();
}

void UI::Implementation::SubscribeToWindowEvents()
{
	m_imguiContext->SubscribeToWindowEvents(
	    [this](std::uint32_t message, std::uintptr_t key, bool wantsTextInput) { return m_editorConsoleSystem != nullptr && m_editorConsoleSystem->HandleShortcut(message, key, wantsTextInput); });
}
