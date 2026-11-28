#include "PCH.h"

#include "UIImplementation.h"
#include "ImGui/EditorImGuiContext.h"
#include "Editor/Public/Icons/EditorIconService.h"

#include "Console/EditorConsoleSystem.h"
#include "GameFramework/Public/Rendering/RenderViewCameraData.h"
#include "GameFramework/Public/Scene/Camera/CameraInputIntent.h"
#include "Workspace/MainMenuBarPanel.h"
#include "Scene/Inspection/SceneInspectorPanel.h"
#include "Scene/SceneOutlinerPanel.h"
#include "Settings/SettingsPanel.h"
#include "Settings/EngineRenderingSettingsController.h"
#include "Viewport/ViewportPanel.h"
#include "Viewport/ViewportToolbar.h"
#include "Renderer/Public/UI/UiTextureHandle.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "ImGui/ImGuiRenderPacketBuilder.h"
#include "Renderer/Public/UI/UiRenderPacket.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"
#include "Viewport/EditorViewportSession.h"
#include "Window/Window.h"

#include <imgui.h>

#include <memory>
#include <stdexcept>
#include <utility>

void UI::Implementation::AppendConsoleOutput(ConsoleOutputRecord record, bool bShowConsole)
{
	m_editorConsoleSystem->AppendOutput(std::move(record));
	if (bShowConsole)
	{
		m_editorConsoleSystem->OpenConsole();
	}
}

const ViewportRenderRequest& UI::Implementation::GetViewportRenderRequest() const noexcept
{
	return m_viewportPanel->GetRenderRequest();
}

RenderViewCameraData UI::Implementation::UpdateViewportCamera(const CameraInputIntent& intent, float deltaSeconds)
{
	m_scene.Refresh();
	const EditorSceneModel& model = m_scene.GetModel();
	if (!model.HasWorld())
	{
		return {};
	}

	const RenderViewportExtent extent = GetViewportRenderRequest().Extent;
	m_viewportSession->SynchronizeWorld(model.GetCameras(), model.GetWorldGeneration());
	const RenderViewCameraData camera = m_viewportSession->UpdateCamera(intent, deltaSeconds, extent);
	m_viewportPanel->SetExposureOverrides(m_viewportSession->GetSettings().Exposure);
	return camera;
}

void UI::Implementation::SetViewportOverlay(std::unique_ptr<ViewportOverlay> overlay) noexcept
{
	m_viewportPanel->SetOverlay(std::move(overlay));
}

void UI::Implementation::SetViewportRenderProducts(const ViewportRenderProducts& products) noexcept
{
	m_viewportGeneration = products.GetGeneration();
	m_viewportPanel->SetRenderProducts(products);
}

void UI::Implementation::SetViewportFinalColorTexture(UiTextureHandle texture) noexcept
{
	m_viewportPanel->SetFinalColorTexture(texture);
}

bool UI::Implementation::ConsumeShaderReloadRequest() noexcept
{
	const bool bRequested = m_bShaderReloadRequested;
	m_bShaderReloadRequested = false;
	return bRequested;
}

void UI::Implementation::SetShaderCompileHandler(std::function<ConsoleCommandResult(bool)> handler)
{
	if (handler)
	{
		m_mainMenuBar->SetShaderCompileHandler(
		    [this, compile = handler](bool bGenerateSymbols)
		    {
			    ConsoleCommandResult result = compile(bGenerateSymbols);
			    AppendConsoleOutput({result.Severity, std::move(result.Message)}, true);
		    });
	}
	else
	{
		m_mainMenuBar->SetShaderCompileHandler({});
	}
	m_editorConsoleSystem->SetShaderCompileHandler(std::move(handler));
}

ViewportOutputAction UI::Implementation::ConsumeViewportOutputAction() noexcept
{
	return m_viewportPanel->ConsumeOutputAction();
}

UiRenderPacket UI::Implementation::ConsumeRenderPacket()
{
	return std::move(m_renderPacket);
}

UI::Implementation::Implementation(EditorHostServices hostServices) :
    m_scene(EditorSceneSource{.AcquireReadView = std::move(hostServices.AcquireWorldReadView), .MaterialVariants = std::move(hostServices.MaterialVariants)}, std::move(hostServices.SubmitWorldEdit)),
    m_timer(hostServices.GameTimer),
    m_levelSession(hostServices.Levels),
    m_window(hostServices.HostWindow),
    m_inputSystem(hostServices.Input)
{
	m_renderPacketBuilder = std::make_unique<ImGuiRenderPacketBuilder>();

	InitializeRenderingSettings(hostServices);
	InitializeConsole(std::move(hostServices.ConsoleVariables));

	InitializeImGuiContext();
	m_imguiContext->ApplyDpiScale(m_window.GetDpiScale());
	if (!m_imguiContext->InitializePlatformBackend())
	{
		throw std::runtime_error("Editor platform backend initialization failed.");
	}

	InitializeWorkspace(std::move(hostServices.RequestRestart));
	SubscribeToWindowEvents();
}

UI::Implementation::~Implementation() noexcept
{
	m_imguiContext->UnsubscribeFromWindowEvents();
}

void UI::Implementation::Update()
{
	NewFrame();
	Build();
	m_renderPacket = m_renderPacketBuilder->Build(*ImGui::GetDrawData(), UiPresentationMode::Viewport, m_viewportGeneration);
}
