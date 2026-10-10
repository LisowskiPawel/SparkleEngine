#include "PCH.h"
#include "GameApplication.h"
#include "ApplicationGraphicsLaunch.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Process/ProcessReadiness.h"
#include "Window/Window.h"
#include "Renderer.h"
#include "GameConsole/GameConsoleHost.h"
#include "Input/InputSystem.h"
#include "World/GameWorld.h"
#include "Input/CameraInputIntentCollector.h"
#include "Level/LevelSession.h"
#include "Time/Timer.h"
#include "Concurrency/ApplicationTaskRuntime.h"
#include "Concurrency/ConcurrencyLaunchCVars.h"
#include "CVars/RHICVars.h"
#include "GameFramework/Public/Rendering/RenderFrameSubmission.h"
#include "GameFramework/Public/Rendering/RenderViewCameraData.h"
#include "GameFramework/Public/Scene/Camera/CameraInputIntent.h"
#include "Renderer/Public/Concurrency/RendererExecutionConfig.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

#include <imgui.h>

#include <cstdint>
#include <memory>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogApplicationConcurrency, "Application.Concurrency");

bool GameApplication::WantsImGuiInputCapture() noexcept
{
	const ImGuiContext* currentContext = ImGui::GetCurrentContext();
	return currentContext != nullptr && (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse);
}

GameApplication::GameApplication() = default;

GameApplication::GameApplication(GameApplicationOptions options) noexcept :
    m_options(options)
{
}

GameApplication::~GameApplication() = default;

Timer& GameApplication::GetTimer() noexcept
{
	return *m_timer;
}

Window& GameApplication::GetWindow() noexcept
{
	return *m_window;
}

InputSystem& GameApplication::GetInputSystem() noexcept
{
	return *m_inputSystem;
}

GameWorld& GameApplication::GetWorldForEditor() noexcept
{
	return *m_gameWorld;
}

LevelSession* GameApplication::GetLevelSession() const noexcept
{
	return m_levelSession.get();
}

Renderer& GameApplication::GetRenderer() noexcept
{
	return *m_renderer;
}

TaskExecutor& GameApplication::GetTaskExecutor() noexcept
{
	return m_taskRuntime->GetExecutor();
}

TaskScope& GameApplication::GetApplicationTaskScope() noexcept
{
	return m_taskRuntime->GetApplicationScope();
}

void GameApplication::Initialize()
{
	if (m_isInitialized)
	{
		return;
	}

	InitializeHost();
	InitializeInput();
	InitializeGameWorld();
	InitializeRenderer();
	InitializeGameConsole();
	m_isInitialized = true;
}

void GameApplication::InitializeHost()
{
	m_timer = std::make_unique<Timer>();
	m_window = std::make_unique<Window>("Sparkle Engine");
}

void GameApplication::InitializeInput()
{
	m_inputSystem = InputSystem::Create();
	m_inputSystem->SetInputCaptureQuery(WantsImGuiInputCapture);
	m_inputSystem->SubscribeToWindow(*m_window);
}

void GameApplication::InitializeGameWorld()
{
	m_taskRuntime = std::make_unique<ApplicationTaskRuntime>();
	m_gameWorld = std::make_unique<GameWorld>(m_taskRuntime->GetExecutor());
	m_cameraInputIntentCollector = std::make_unique<CameraInputIntentCollector>(*m_inputSystem, *m_window);

	m_levelSession = std::make_unique<LevelSession>(*m_gameWorld, m_taskRuntime->GetExecutor(), m_taskRuntime->GetApplicationScope());
	m_levelSession->GetLevelChangeEvents().OnLevelChanged.Add([](const LevelChangedEventArgs& event) noexcept { Process::SignalParentReadiness(event.activeLevelName); });
}

void GameApplication::InitializeRenderer()
{
	const RendererGraphicsLaunch graphicsLaunch = ResolveApplicationGraphicsLaunch();
	RendererExecutionConfig rendererConfig;
	if (m_options.AllowThreadedRenderer && ConcurrencyLaunchCVars::UseThreadedRenderer())
	{
		rendererConfig.Mode = RendererExecutionMode::Threaded;
		rendererConfig.RenderPipelineDepth = ConcurrencyLaunchCVars::ResolveRenderPipelineDepth();
		if (rendererConfig.RenderPipelineDepth >= CVarMaximumFramesInFlight.Get())
		{
			Diagnostics::Fatal(
			    LogApplicationConcurrency,
			    __FILE__,
			    __LINE__,
			    "r.RenderPipelineDepth must be lower than r.MaximumFramesInFlight so queued simulation frames have distinct GPU frame "
			    "slots.");
		}
	}

	rendererConfig.EnableUiRenderPackets = m_options.EnableUiRenderPackets || m_options.EnableGameConsole;
	rendererConfig.AssetTaskExecutor = &m_taskRuntime->GetExecutor();
	rendererConfig.AssetTaskParentScope = &m_taskRuntime->GetApplicationScope();
	m_renderer = std::make_unique<Renderer>(*m_timer, *m_window, graphicsLaunch, rendererConfig);
}

void GameApplication::InitializeGameConsole()
{
	if (m_options.EnableGameConsole)
	{
		m_gameConsoleHost = std::make_unique<GameConsoleHost>(*m_timer, *m_window, [this](CVarControlRequest request) { return m_renderer->ExecuteConsoleVariables(std::move(request)); });
	}
}

GameApplicationFrameResult GameApplication::BeginFrame()
{
	if (!m_isInitialized)
	{
		return GameApplicationFrameResult::Exit;
	}

	m_inputSystem->BeginFrame();
	m_window->PollEvents();
	m_inputSystem->ProcessDeferredEvents();
	m_timer->Tick();

	if (m_window->ShouldClose())
	{
		return GameApplicationFrameResult::Exit;
	}

	if (m_window->IsMinimized() || !m_window->HasValidSize())
	{
		m_window->WaitForEvent();
		return GameApplicationFrameResult::SkipRender;
	}

	if (m_levelSession)
	{
		m_levelSession->ProcessPendingLevelChange();
	}

	return GameApplicationFrameResult::Ready;
}

void GameApplication::UpdateGame() noexcept
{
	const float width = static_cast<float>(m_window->GetWidth());
	const float height = static_cast<float>(m_window->GetHeight());
	const CameraInputIntent cameraIntent = CollectCameraInputIntent(height > 0.0f ? width / height : 1.0f);
	UpdateGameFrame(&cameraIntent, nullptr);
}

CameraInputIntent GameApplication::CollectCameraInputIntent(float aspectRatio) noexcept
{
	return m_cameraInputIntentCollector != nullptr ? m_cameraInputIntentCollector->Collect(aspectRatio) : CameraInputIntent{};
}

void GameApplication::UpdateEditorGame(const RenderViewCameraData& renderCamera) noexcept
{
	UpdateGameFrame(nullptr, &renderCamera);
}

void GameApplication::UpdateGameFrame(const CameraInputIntent* worldCameraIntent, const RenderViewCameraData* renderCameraOverride) noexcept
{
	if (m_gameWorld && m_timer)
	{
		const std::uint64_t frameId = m_timer->GetFrameCount();
		m_renderer->BeginSimulationFrame(frameId);
		m_gameWorld->PublishCameraInputIntent(worldCameraIntent != nullptr ? *worldCameraIntent : CameraInputIntent{});
		const float deltaSeconds = static_cast<float>(m_timer->GetDelta(TimeDomain::Scaled, TimeUnit::Seconds));
		m_gameWorld->Update(deltaSeconds);
		m_renderer->EndSimulationFrame(frameId);
		SubmitRenderFrame(frameId, renderCameraOverride);
	}
}

void GameApplication::SubmitRenderFrame(std::uint64_t frameId, const RenderViewCameraData* renderCameraOverride)
{
	RenderFrameSubmission submission = m_gameWorld->ExtractRenderFrameSubmission(frameId);
	if (renderCameraOverride != nullptr)
	{
		submission.View.Camera = *renderCameraOverride;
	}
	m_renderer->SubmitRenderFrame(std::move(submission));
}

void GameApplication::SubmitViewportRenderRequest(ViewportRenderRequest request) noexcept
{
	if (m_renderer)
	{
		m_renderer->SubmitViewportRenderRequest(request);
	}
}

bool GameApplication::Tick()
{
	switch (BeginFrame())
	{
		case GameApplicationFrameResult::Exit:
			return false;
		case GameApplicationFrameResult::SkipRender:
			return true;
		case GameApplicationFrameResult::Ready:
		default:
			break;
	}

	UpdateGame();
	if (m_gameConsoleHost != nullptr)
	{
		if (auto packet = m_gameConsoleHost->Update())
		{
			m_renderer->SubmitUiRenderPacket(std::move(*packet));
		}
	}
	m_renderer->OnRender();
	return true;
}

void GameApplication::Shutdown()
{
	if (!m_isInitialized)
	{
		return;
	}

	m_gameConsoleHost.reset();
	m_renderer.reset();
	m_levelSession.reset();
	m_cameraInputIntentCollector.reset();
	m_gameWorld.reset();
	m_taskRuntime.reset();
	m_inputSystem.reset();
	m_window.reset();
	m_timer.reset();
	m_isInitialized = false;
}
