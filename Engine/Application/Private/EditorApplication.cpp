#include "PCH.h"
#include "EditorApplication.h"
#include "Editor/EditorApplicationState.h"

#include "Editor/EditorUiFrameRenderer.h"
#if SPARKLE_APPLICATION_WITH_EXTERNAL_CAPTURE_UI
  #include "Editor/ExternalCapture/ExternalCaptureUiComposition.h"
#endif
#include "Editor/Public/UI.h"
#include "Editor/Viewport/EditorViewportOutputCoordinator.h"
#include "EditorOperations/EditorOperationRuntime.h"
#include "Editor/Shaders/EditorShaderCompileOperation.h"
#include "Input/InputSystem.h"
#include "Renderer.h"
#include "GameApplication.h"
#include "Time/Timer.h"
#include "Window/Window.h"

#include <utility>

EditorApplication::EditorApplication() = default;

EditorApplication::EditorApplication(GameApplicationOptions options, EditorShaderCompileAction compileShaders) noexcept :
    m_gameOptions(std::move(options)),
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
	if (!m_state)
	{
		m_state = std::make_unique<State>();
	}

	InitializeGameApplication();
	if (m_state->Game->GetWindow().ShouldClose())
	{
		return;
	}

	InitializeEditorOperations();
	InitializeUi();
#if SPARKLE_APPLICATION_WITH_EXTERNAL_CAPTURE_UI
	m_state->Ui->SetViewportOverlay(CreateEditorExternalCaptureOverlay(m_state->Game->GetRenderer()));
#endif
	m_bIsEditorSessionActive = true;
}

bool EditorApplication::Tick()
{
	if (!m_bIsEditorSessionActive || !m_state->Game || !m_state->Ui)
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
	m_state->Game->UpdateEditorGame(UpdateViewportCamera());
	RenderEditorFrame(renderer);
	return true;
}

void EditorApplication::UpdateEditorOperations(Renderer& renderer)
{
	m_state->ViewportOutput->Update(*m_state->Ui, renderer);
	if (m_state->ShaderCompileOperation)
	{
		m_state->ShaderCompileOperation->Update(*m_state->Ui, renderer);
	}

	if (m_state->Ui->ConsumeShaderReloadRequest())
	{
		ReloadShaders(renderer);
	}
}

void EditorApplication::RenderEditorFrame(Renderer& renderer)
{
	EditorUiFrameRenderer::Render(renderer, *m_state->Ui);
	m_state->ViewportOutput->HandleAction(*m_state->Ui, renderer, m_state->Game->GetTimer().GetFrameCount());

	m_state->Game->SubmitViewportRenderRequest(m_state->Ui->GetViewportRenderRequest());
}

void EditorApplication::Shutdown()
{
	if (!m_state)
	{
		return;
	}
	m_state->Ui.reset();
	m_state->ViewportOutput.reset();
	m_state->ShaderCompileOperation.reset();
	m_state->OperationRuntime.reset();
	if (m_state->Game)
	{
		m_state->Game->Shutdown();
	}
	m_bIsEditorSessionActive = false;
}
