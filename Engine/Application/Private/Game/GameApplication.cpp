#include "PCH.h"
#include "Game/GameApplication.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "Diagnostics/GpuCrash/GpuCrashIncidentSink.h"
#endif
#include "Core/Public/Console/CVar.h"
#include "GameFramework/Public/Rendering/RenderFrameSubmission.h"
#include "GameFramework/Public/Rendering/RenderViewCameraData.h"
#include "GameFramework/Public/Scene/Camera/CameraInputIntent.h"
#include "Input/CameraInputIntentCollector.h"
#include "Input/InputSystem.h"
#include "Level/LevelSession.h"
#include "Renderer.h"
#include "Renderer/Public/Concurrency/RendererExecutionConfig.h"
#include "Startup/ApplicationGraphicsLaunch.h"
#include "Time/Timer.h"
#include "Window/Window.h"
#include "World/GameWorld.h"

#include <cstdint>
#include <memory>
#include <utility>

namespace ConcurrencyLaunchCVars
{
	static ConsoleVariable<bool> g_bThreadedRenderer("r.ThreadedRenderer", true, "Run renderer/RHI ownership on Sparkle.RenderThread.");
	static ConsoleVariable<std::uint32_t> g_renderPipelineDepth("r.RenderPipelineDepth", 1, "Bounded renderer CPU lead in frames. Supported values are 0 through 2.");
}

GameApplication::GameApplication() = default;

GameApplication::GameApplication(GameApplicationOptions options) noexcept :
    m_options(std::move(options))
{
}

GameApplication::~GameApplication()
{
	Shutdown();
}

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

void GameApplication::Initialize()
{
	if (m_bIsInitialized)
	{
		return;
	}

	try
	{
		InitializeExecution();
		InitializeHost();
		InitializeInput();
		InitializeGameWorld();
		InitializeRenderer();
		if (!m_renderer)
		{
			Shutdown();
			return;
		}
	}
	catch (...)
	{
		Shutdown();
		throw;
	}

	m_bIsInitialized = true;
}

GameApplicationFrameResult GameApplication::BeginFrame()
{
	if (!m_bIsInitialized)
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

#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	ReportReadiness();
#endif
	return GameApplicationFrameResult::Ready;
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
	m_renderer->OnRender();
	return true;
}

void GameApplication::Shutdown()
{
	m_renderer.reset();
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	m_gpuCrashSink.reset();
#endif
	m_stop.request_stop();
	m_levelSession.reset();
	m_cameraInputIntentCollector.reset();
	m_gameWorld.reset();
	ShutdownExecution();
	m_inputSystem.reset();
	m_window.reset();
	m_timer.reset();
	m_bIsInitialized = false;
}

void GameApplication::InitializeHost()
{
	m_timer = std::make_unique<Timer>();
	m_window = std::make_unique<Window>(m_options.WindowTitle);
}

void GameApplication::InitializeInput()
{
	m_inputSystem = std::make_unique<InputSystem>();
	m_inputSystem->SubscribeToWindow(*m_window);
}

void GameApplication::InitializeGameWorld()
{
	m_gameWorld = std::make_unique<GameWorld>(m_frameScheduler);
	m_cameraInputIntentCollector = std::make_unique<CameraInputIntentCollector>(*m_inputSystem, *m_window);

	m_levelSession = std::make_unique<LevelSession>(*m_gameWorld, m_ioScheduler, m_backgroundScheduler, m_stop.get_token());
	auto& levelChangeEvents = m_levelSession->GetLevelChangeEvents();
	levelChangeEvents.OnLevelChanged.Add([this](const LevelChangedEventArgs& event) { SignalLevelReadiness(event.activeLevelName); });
}

void GameApplication::InitializeRenderer()
{
	RendererExecutionConfig execution;
	if (ConcurrencyLaunchCVars::g_bThreadedRenderer.Get())
	{
		execution.Mode = RendererExecutionMode::Threaded;
		execution.RenderPipelineDepth = ConcurrencyLaunchCVars::g_renderPipelineDepth.Get();
	}
	execution.bEnableUiRenderPackets = m_options.bEnableUiRenderPackets;
	execution.FrameScheduler = m_frameScheduler;
	execution.IoScheduler = m_ioScheduler;
	execution.BackgroundScheduler = m_backgroundScheduler;

	RendererGraphicsLaunch graphics = ResolveApplicationGraphicsLaunch();
	try
	{
		PrepareGpuCrashDiagnostics(graphics);
		m_renderer = Renderer::Create(*m_timer, *m_window, graphics, execution);
	}
	catch (...)
	{
		AbortGpuCrashDiagnostics();
		throw;
	}
}

void GameApplication::UpdateGame()
{
	const CameraInputIntent cameraIntent = m_cameraInputIntentCollector->Collect();
	UpdateGameFrame(&cameraIntent, nullptr);
}

CameraInputIntent GameApplication::CollectCameraInputIntent(float aspectRatio) noexcept
{
	return m_cameraInputIntentCollector != nullptr ? m_cameraInputIntentCollector->Collect(aspectRatio) : CameraInputIntent{};
}

void GameApplication::UpdateEditorGame(const RenderViewCameraData& renderCamera)
{
	UpdateGameFrame(nullptr, &renderCamera);
}

void GameApplication::UpdateGameFrame(const CameraInputIntent* worldCameraIntent, const RenderViewCameraData* renderCameraOverride)
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
