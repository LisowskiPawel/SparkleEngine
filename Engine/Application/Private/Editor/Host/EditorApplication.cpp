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
	auto overlay = CreateEditorExternalCaptureOverlay(m_game->GetRenderer());
	m_ui->SetViewportOverlay(std::move(overlay));
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
	const RenderViewCameraData camera = UpdateViewportCamera();
	m_game->UpdateEditorGame(camera);
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

	auto renderPacket = m_ui->ConsumeRenderPacket();
	renderer.SubmitUiRenderPacket(std::move(renderPacket));
	renderer.OnRender();

	const std::uint64_t frameId = m_game->GetTimer().GetFrameCount();
	m_viewportOutput->HandleAction(*m_ui, renderer, frameId);
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
		TaskScopeDesc scopeDesc{TaskScopeKind::Document, "Editor operations"};
		TaskScope& applicationScope = m_game->GetApplicationTaskScope();
		m_operationScope = std::make_unique<TaskScope>(std::move(scopeDesc), &applicationScope);
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

	EditorHostServices hostServices{
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
	    .Input = m_game->GetInputSystem()};

	m_ui = std::make_unique<UI>(std::move(hostServices));

	ConfigureUiDiagnostics(renderer);
	if (m_shaderCompileOperation)
	{
		m_ui->SetShaderCompileHandler([this](bool bGenerateSymbols) { return m_shaderCompileOperation->Request(bGenerateSymbols); });
	}
}

void EditorApplication::ConfigureUiDiagnostics(Renderer& renderer)
{
	EditorDiagnosticsProviders providers{
	    .Shaders = [&renderer]() { return renderer.CaptureShaderDiagnostics(); },
	    .MeshDiagnostics = [&renderer]() { return renderer.CaptureMeshDiagnostics(); },
	    .TextureDiagnostics = [&renderer]() { return renderer.CaptureTextureDiagnostics(); },
	    .MeshPreview = [&renderer](std::uintptr_t meshRuntimeId) { return renderer.CaptureMeshPreview(meshRuntimeId); }};

	m_ui->SetDiagnosticsProviders(std::move(providers));
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
	const RenderViewportExtent& extent = viewportRequest.Extent;
	const float aspectRatio = extent.IsValid() ? static_cast<float>(extent.Width) / static_cast<float>(extent.Height) : 1.0f;
	const CameraInputIntent cameraIntent = m_game->CollectCameraInputIntent(aspectRatio);
	const Timer& timer = m_game->GetTimer();
	const float deltaSeconds = static_cast<float>(timer.GetDelta(TimeDomain::Scaled, TimeUnit::Seconds));
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
