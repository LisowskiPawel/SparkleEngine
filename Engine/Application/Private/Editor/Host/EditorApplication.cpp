#include "PCH.h"
#include "Editor/Host/EditorApplication.h"
#include "Renderer/Public/Viewport/ViewportPresentationSnapshot.h"

#if SPARKLE_APPLICATION_WITH_EXTERNAL_CAPTURE_UI
  #include "Editor/ExternalCapture/ExternalCaptureUiComposition.h"
#endif
#include "Editor/Public/UI.h"
#include "Editor/Viewport/EditorViewportOutputCoordinator.h"
#include "Editor/Shaders/EditorShaderCompileOperation.h"
#include "Game/GameApplication.h"
#include "GameFramework/Public/Rendering/RenderViewCameraData.h"
#include "GameFramework/Public/Scene/Camera/CameraInputIntent.h"
#include "RenderingSettings/EngineRenderingSettingsPersistence.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Input/InputSystem.h"
#include "Renderer.h"
#include "Tasks/Public/TaskScope.h"
#include "Time/Timer.h"
#include "Window/Window.h"
#include "World/GameWorld.h"

#include <chrono>
#include <utility>

EditorApplication::EditorApplication(EditorShaderCompileAction compileShaders) noexcept :
    m_compileShaders(std::move(compileShaders))
{
}

EditorApplication::~EditorApplication()
{
	Shutdown();
}

void EditorApplication::Initialize()
{
	if (m_bIsEditorSessionActive)
	{
		return;
	}

	InitializeGameApplication();
	if (m_game->GetWindow().ShouldClose())
	{
		return;
	}

	InitializeEditorOperations();
	InitializeUi();
#if SPARKLE_APPLICATION_WITH_EXTERNAL_CAPTURE_UI
	m_ui->SetViewportOverlay(CreateEditorExternalCaptureOverlay(m_game->GetRenderer()));
#endif
	m_bIsEditorSessionActive = true;
}

bool EditorApplication::Tick()
{
	if (!m_bIsEditorSessionActive)
	{
		return false;
	}

	switch (m_game->BeginFrame())
	{
		case GameApplicationFrameResult::Exit:
			return false;
		case GameApplicationFrameResult::SkipRender:
			return true;
		case GameApplicationFrameResult::Ready:
		default:
			break;
	}

	Renderer& renderer = m_game->GetRenderer();
	UpdateEditorOperations(renderer);
	m_game->UpdateEditorGame(UpdateViewportCamera());
	RenderEditorFrame(renderer);
	return true;
}

void EditorApplication::UpdateEditorOperations(Renderer& renderer)
{
	m_viewportOutput->Update(*m_ui, renderer);
	if (m_shaderCompileOperation)
	{
		m_shaderCompileOperation->Update(*m_ui, renderer);
	}

	if (m_ui->ConsumeShaderReloadRequest())
	{
		ReloadShaders(renderer);
	}
}

void EditorApplication::RenderEditorFrame(Renderer& renderer)
{
	const ViewportPresentationSnapshot presentation = renderer.GetViewportPresentation();
	m_ui->SetViewportRenderProducts(presentation.Products);
	m_ui->SetViewportFinalColorTexture(presentation.Texture);
	m_ui->Update();

	renderer.SubmitUiRenderPacket(m_ui->ConsumeRenderPacket());
	renderer.OnRender();

	m_viewportOutput->HandleAction(*m_ui, renderer, m_game->GetTimer().GetFrameCount());
	m_game->SubmitViewportRenderRequest(m_ui->GetViewportRenderRequest());
}

void EditorApplication::Shutdown()
{
	if (m_operationScope)
	{
		m_operationScope->Cancel();
		(void) m_operationScope->JoinFor(std::chrono::milliseconds::max());
	}

	m_ui.reset();
	m_viewportOutput.reset();
	m_shaderCompileOperation.reset();
	m_operationScope.reset();
	m_game.reset();
	m_bIsEditorSessionActive = false;
}

static void SaveAndSubmitRenderingSettings(Renderer& renderer, EngineRenderingSettingsState settings)
{
	EngineRenderingSettingsPersistence::Write(settings);
	renderer.SubmitRenderingSettings(std::move(settings));
}

void EditorApplication::InitializeGameApplication()
{
	if (!m_game)
	{
		m_game = std::make_unique<GameApplication>(GameApplicationOptions{.WindowTitle = "Sparkle Editor", .bEnableUiRenderPackets = true});
	}

	m_game->Initialize();
}

void EditorApplication::InitializeEditorOperations()
{
	if (!m_operationScope)
	{
		m_operationScope = std::make_unique<TaskScope>(TaskScopeDesc{TaskScopeKind::Document, "Editor operations"}, &m_game->GetApplicationTaskScope());
	}

	if (!m_viewportOutput)
	{
		m_viewportOutput = std::make_unique<EditorViewportOutputCoordinator>(m_game->GetTaskExecutor(), *m_operationScope);
	}

	if (!m_shaderCompileOperation && m_compileShaders)
	{
		m_shaderCompileOperation = std::make_unique<EditorShaderCompileOperation>(m_game->GetTaskExecutor(), *m_operationScope, m_compileShaders);
	}
}

void EditorApplication::InitializeUi()
{
	m_game->GetInputSystem().BeginInputRoutingFrame(false, false);
	if (m_ui)
	{
		return;
	}

	Renderer& renderer = m_game->GetRenderer();
	GameWorld& world = m_game->GetWorldForEditor();

	m_ui = std::make_unique<UI>(EditorHostServices{
	    .GameTimer = m_game->GetTimer(),
	    .Levels = m_game->GetLevelSession(),
	    .AcquireWorldReadView = [&world]() { return world.AcquireReadView(); },
	    .MaterialVariants = [&world]() { return world.CaptureMaterialVariants(); },
	    .SubmitWorldEdit = [&world](WorldEditCommand command, std::uint64_t generation) { return world.SubmitEdit(std::move(command), generation); },
	    .RenderingSettings = renderer.CaptureRenderingSettings(),
	    .SubmitRenderingSettings = [&renderer](EngineRenderingSettingsState settings) { SaveAndSubmitRenderingSettings(renderer, std::move(settings)); },
	    .CaptureRenderingSettings = [&renderer]() { return renderer.CaptureRenderingSettings(); },
	    .ConsoleVariables = [&renderer](CVarControlRequest request) { return renderer.ExecuteConsoleVariables(std::move(request)); },
	    .RequestRestart = [this]() { Restart(); },
	    .HostWindow = m_game->GetWindow(),
	    .Input = m_game->GetInputSystem()});

	ConfigureUiDiagnostics(renderer);
	if (m_shaderCompileOperation)
	{
		m_ui->SetShaderCompileHandler([this](bool bGenerateSymbols) { return m_shaderCompileOperation->Request(bGenerateSymbols); });
	}
}

void EditorApplication::ConfigureUiDiagnostics(Renderer& renderer)
{
	m_ui->SetDiagnosticsProviders(
	    EditorDiagnosticsProviders{
	        .Shaders = [&renderer]() { return renderer.CaptureShaderDiagnostics(); },
	        .MeshDiagnostics = [&renderer]() { return renderer.CaptureMeshDiagnostics(); },
	        .TextureDiagnostics = [&renderer]() { return renderer.CaptureTextureDiagnostics(); },
	        .MeshPreview = [&renderer](std::uintptr_t meshRuntimeId) { return renderer.CaptureMeshPreview(meshRuntimeId); }});
}

void EditorApplication::Restart()
{
	if (Process::RelaunchCurrentProcess())
	{
		m_game->GetWindow().RequestClose();
	}
	else
	{
		m_ui->AppendConsoleOutput({ConsoleCommandSeverity::Error, "Editor restart could not launch a new process."}, true);
	}
}

RenderViewCameraData EditorApplication::UpdateViewportCamera()
{
	const ViewportRenderRequest& viewportRequest = m_ui->GetViewportRenderRequest();
	const float aspectRatio = viewportRequest.Extent.IsValid() ? static_cast<float>(viewportRequest.Extent.Width) / static_cast<float>(viewportRequest.Extent.Height) : 1.0f;
	const CameraInputIntent cameraIntent = m_game->CollectCameraInputIntent(aspectRatio);
	const float deltaSeconds = static_cast<float>(m_game->GetTimer().GetDelta(TimeDomain::Scaled, TimeUnit::Seconds));
	const RenderViewCameraData renderCamera = m_ui->UpdateViewportCamera(cameraIntent, deltaSeconds);
	return renderCamera;
}

void EditorApplication::ReloadShaders(Renderer& renderer)
{
	try
	{
		renderer.ReloadShaders();
	}
	catch (const Diagnostics::Error& error)
	{
		m_ui->AppendConsoleOutput({ConsoleCommandSeverity::Error, error.what()});
	}
}
