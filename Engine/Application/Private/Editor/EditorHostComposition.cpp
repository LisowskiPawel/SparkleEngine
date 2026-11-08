#include "PCH.h"
#include "EditorApplication.h"
#include "Editor/EditorApplicationState.h"
#include "Editor/Viewport/EditorViewportOutputCoordinator.h"
#include "EditorOperations/EditorOperationRuntime.h"

#include "Editor/Public/UI.h"
#include "Editor/Shaders/EditorShaderCompileOperation.h"
#include "RenderingSettings/EngineRenderingSettingsPersistence.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Input/InputSystem.h"
#include "Renderer.h"
#include "Time/Timer.h"
#include "Window/Window.h"
#include "World/GameWorld.h"

#include <utility>

static void SaveAndSubmitRenderingSettings(Renderer& renderer, EngineRenderingSettingsState settings)
{
	EngineRenderingSettingsPersistence::Write(settings);
	renderer.SubmitRenderingSettings(std::move(settings));
}

void EditorApplication::InitializeGameApplication()
{
	if (!m_state->Game)
	{
		GameApplicationOptions gameOptions = m_gameOptions;
		gameOptions.WindowTitle = "Sparkle Editor";
		gameOptions.bEnableUiRenderPackets = true;
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

	if (!m_state->ViewportOutput)
	{
		m_state->ViewportOutput = std::make_unique<EditorViewportOutputCoordinator>(*m_state->OperationRuntime);
	}
	if (!m_state->ShaderCompileOperation && m_compileShaders)
	{
		m_state->ShaderCompileOperation = std::make_unique<EditorShaderCompileOperation>(*m_state->OperationRuntime, m_compileShaders);
	}
}

void EditorApplication::InitializeUi()
{
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
	    .MaterialVariants = [&world]() { return world.CaptureMaterialVariants(); },
	    .SubmitWorldEdit = [&world](WorldEditCommand command, std::uint64_t generation) { return world.SubmitEdit(std::move(command), generation); },
	    .RenderingSettings = renderer.CaptureRenderingSettings(),
	    .SubmitRenderingSettings = [&renderer](EngineRenderingSettingsState settings) { SaveAndSubmitRenderingSettings(renderer, std::move(settings)); },
	    .CaptureRenderingSettings = [&renderer]() { return renderer.CaptureRenderingSettings(); },
	    .ConsoleVariables = [&renderer](CVarControlRequest request) { return renderer.ExecuteConsoleVariables(std::move(request)); },
	    .RequestRestart = [this]() { Restart(); },
	    .HostWindow = m_state->Game->GetWindow(),
	    .Input = m_state->Game->GetInputSystem()});

	ConfigureUiDiagnostics(renderer);
	if (m_state->ShaderCompileOperation)
	{
		m_state->Ui->SetShaderCompileHandler([this](bool bGenerateSymbols) { return m_state->ShaderCompileOperation->Request(bGenerateSymbols); });
	}
}

void EditorApplication::ConfigureUiDiagnostics(Renderer& renderer)
{
	m_state->Ui->SetDiagnosticsProviders(
	    EditorDiagnosticsProviders{
	        .Shaders = [&renderer]() { return renderer.CaptureShaderDiagnostics(); },
	        .MeshDiagnostics = [&renderer]() { return renderer.CaptureMeshDiagnostics(); },
	        .TextureDiagnostics = [&renderer]() { return renderer.CaptureTextureDiagnostics(); },
	        .MemoryDiagnostics = [&renderer]() { return renderer.CaptureMemoryDiagnostics(); },
	        .MeshPreview = [&renderer](std::uintptr_t meshRuntimeId) { return renderer.CaptureMeshPreview(meshRuntimeId); }});
}

void EditorApplication::Restart()
{
	if (Process::RelaunchCurrentProcess())
	{
		m_state->Game->GetWindow().RequestClose();
	}
	else
	{
		m_state->Ui->AppendConsoleOutput({ConsoleCommandSeverity::Error, "Editor restart could not launch a new process."}, true);
	}
}

RenderViewCameraData EditorApplication::UpdateViewportCamera()
{
	const ViewportRenderRequest& viewportRequest = m_state->Ui->GetViewportRenderRequest();
	const float aspectRatio = viewportRequest.Extent.IsValid() ? static_cast<float>(viewportRequest.Extent.Width) / static_cast<float>(viewportRequest.Extent.Height) : 1.0f;
	const CameraInputIntent cameraIntent = m_state->Game->CollectCameraInputIntent(aspectRatio);
	const float deltaSeconds = static_cast<float>(m_state->Game->GetTimer().GetDelta(TimeDomain::Scaled, TimeUnit::Seconds));
	const RenderViewCameraData renderCamera = m_state->Ui->UpdateViewportCamera(cameraIntent, deltaSeconds);
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
		m_state->Ui->AppendConsoleOutput({ConsoleCommandSeverity::Error, error.what()});
	}
}
