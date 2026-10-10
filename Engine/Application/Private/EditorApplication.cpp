#include "PCH.h"
#include "EditorApplication.h"

#include "Editor/EditorUiFrameRenderer.h"
#if SPARKLE_APPLICATION_WITH_EXTERNAL_CAPTURE_UI
  #include "Editor/ExternalCapture/ExternalCaptureUiComposition.h"
#endif
#include "Editor/Public/UI.h"
#include "Editor/Viewport/EditorViewportOutputCoordinator.h"
#include "EditorOperations/EditorOperationRuntime.h"
#include "Input/InputSystem.h"
#include "Renderer.h"
#include "GameApplication.h"
#include "ShaderRecook/ShaderConsoleCommands.h"
#include "ShaderRecook/ShaderRecookCoordinator.h"
#include "Time/Timer.h"
#include "World/GameWorld.h"

#include <utility>

struct EditorApplication::State final
{
	std::unique_ptr<GameApplication> Game;
	std::unique_ptr<UI> Ui;
	std::unique_ptr<EditorOperationRuntime> OperationRuntime;
	std::unique_ptr<ShaderRecookCoordinator> ShaderRecook;
	std::unique_ptr<EditorViewportOutputCoordinator> ViewportOutput;
};

EditorApplication::EditorApplication() = default;

EditorApplication::EditorApplication(GameApplicationOptions options) noexcept :
    m_gameOptions(options)
{
}

EditorApplication::~EditorApplication() = default;

void EditorApplication::Initialize()
{
	if (m_isEditorSessionActive)
	{
		return;
	}
	if (!m_state)
	{
		m_state = std::make_unique<State>();
	}

	InitializeGameApplication();
	InitializeEditorOperations();
	InitializeUi();
#if SPARKLE_APPLICATION_WITH_EXTERNAL_CAPTURE_UI
	m_state->Ui->SetViewportOverlay(CreateEditorExternalCaptureOverlay(m_state->Game->GetRenderer()));
#endif
	m_isEditorSessionActive = true;
}

void EditorApplication::InitializeGameApplication()
{
	if (!m_state->Game)
	{
		GameApplicationOptions gameOptions = m_gameOptions;
		gameOptions.EnableGameConsole = false;
		gameOptions.EnableUiRenderPackets = true;
		m_state->Game = std::make_unique<GameApplication>(gameOptions);
	}

	m_state->Game->Initialize();
}

void EditorApplication::InitializeEditorOperations()
{
	if (!m_state->OperationRuntime)
	{
		m_state->OperationRuntime = std::make_unique<EditorOperationRuntime>(m_state->Game->GetTaskExecutor(), m_state->Game->GetApplicationTaskScope());
	}

	if (!m_state->ShaderRecook)
	{
		m_state->ShaderRecook = std::make_unique<ShaderRecookCoordinator>(*m_state->OperationRuntime);
	}

	if (!m_state->ViewportOutput)
	{
		m_state->ViewportOutput = std::make_unique<EditorViewportOutputCoordinator>(*m_state->OperationRuntime);
	}
}

void EditorApplication::InitializeUi()
{
	m_state->Game->GetInputSystem().ClearInputCaptureQuery();
	m_state->Game->GetInputSystem().BeginInputRoutingFrame(false, false);
	if (m_state->Ui)
	{
		return;
	}

	Renderer& renderer = m_state->Game->GetRenderer();
	GameWorld& world = m_state->Game->GetWorldForEditor();

	m_state->Ui = std::make_unique<UI>(EditorHostServices{
	    .GameTimer = m_state->Game->GetTimer(),
	    .Levels = m_state->Game->GetLevelSession(),
	    .AcquireWorldReadView = [&world]() { return world.AcquireReadView(); },
	    .ReadWorldChanges = [&world](const WorldChangeCursor& cursor) { return world.ReadChanges(cursor); },
	    .AcknowledgeWorldChanges = [&world](WorldChangeCursor& cursor, WorldSequence sequence) { return world.AcknowledgeChanges(cursor, sequence); },
	    .WorldGeneration = [&world]() noexcept { return world.GetGeneration(); },
	    .MaterialVariants = [&world]() { return world.CaptureMaterialVariants(); },
	    .SubmitWorldEdit = [&world](WorldEditCommand command, std::uint64_t generation) { return world.SubmitEdit(std::move(command), generation); },
	    .RenderingSettings = renderer.CaptureRenderingSettings(),
	    .SubmitRenderingSettings =
	        [&renderer](EngineRenderingSettingsState settings)
	    {
		    SaveRenderingSettings(settings);
		    renderer.SubmitRenderingSettings(std::move(settings));
	    },
	    .CaptureRenderingSettings = [&renderer]() { return renderer.CaptureRenderingSettings(); },
	    .ConsoleVariables = [&renderer](CVarControlRequest request) { return renderer.ExecuteConsoleVariables(std::move(request)); },
	    .HostWindow = m_state->Game->GetWindow(),
	    .Input = m_state->Game->GetInputSystem()});

	ConfigureUiDiagnostics(renderer);
	ShaderConsoleCommands::ConnectEditor(*m_state->Ui, *m_state->ShaderRecook);
}

void EditorApplication::ConfigureUiDiagnostics(Renderer& renderer)
{
	m_state->Ui->SetDiagnosticsProviders(
	    EditorDiagnosticsProviders{
	        .ShaderGeneration = [&renderer]() noexcept { return renderer.GetShaderGeneration(); },
	        .MeshDiagnostics = [&renderer]() { return renderer.CaptureMeshDiagnostics(); },
	        .TextureDiagnostics = [&renderer]() { return renderer.CaptureTextureDiagnostics(); },
	        .MemoryDiagnostics = [&renderer]() { return renderer.CaptureMemoryDiagnostics(); },
	        .MeshPreview = [&renderer](std::uintptr_t meshRuntimeId) { return renderer.CaptureMeshPreview(meshRuntimeId); }});
}

bool EditorApplication::Tick()
{
	if (!m_isEditorSessionActive || !m_state->Game || !m_state->Ui)
	{
		return false;
	}

	switch (m_state->Game->BeginFrame())
	{
		case GameApplicationFrameResult::Exit:
			return false;
		case GameApplicationFrameResult::SkipRender:
			return true;
		case GameApplicationFrameResult::Ready:
		default:
			break;
	}

	Renderer& renderer = m_state->Game->GetRenderer();
	UpdateEditorOperations(renderer);
	const ViewportRenderRequest& viewportRequest = m_state->Ui->GetViewportRenderRequest();
	const float aspectRatio = viewportRequest.Extent.IsValid() ? static_cast<float>(viewportRequest.Extent.Width) / static_cast<float>(viewportRequest.Extent.Height) : 1.0f;
	const CameraInputIntent cameraIntent = m_state->Game->CollectCameraInputIntent(aspectRatio);
	const float deltaSeconds = static_cast<float>(m_state->Game->GetTimer().GetDelta(TimeDomain::Scaled, TimeUnit::Seconds));
	const RenderViewCameraData renderCamera = m_state->Ui->UpdateViewportCamera(cameraIntent, deltaSeconds);
	m_state->Game->UpdateEditorGame(renderCamera);
	RenderEditorFrame(renderer);
	return true;
}

void EditorApplication::UpdateEditorOperations(Renderer& renderer)
{
	m_state->ViewportOutput->Update(renderer);

	if (m_state->Ui->ConsumeShaderRecookRequest())
	{
		m_state->ShaderRecook->RequestRecook();
	}

	m_state->ShaderRecook->Update(renderer, m_state->Ui->ConsumeShaderReloadRequest());
}

void EditorApplication::RenderEditorFrame(Renderer& renderer)
{
	EditorUiFrameRenderer::Render(renderer, *m_state->Ui);
	m_state->ViewportOutput->HandleAction(*m_state->Ui, renderer, m_state->Game->GetTimer().GetFrameCount());

	m_state->Game->SubmitViewportRenderRequest(m_state->Ui->GetViewportRenderRequest());
}

void EditorApplication::Shutdown()
{
	if (!m_isEditorSessionActive)
	{
		return;
	}

	m_state->Ui.reset();
	m_state->ViewportOutput.reset();
	m_state->ShaderRecook.reset();
	m_state->OperationRuntime.reset();
	m_state->Game->Shutdown();
	m_isEditorSessionActive = false;
}
