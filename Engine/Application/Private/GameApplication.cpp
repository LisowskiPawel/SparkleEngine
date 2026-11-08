#include "PCH.h"
#include "GameApplication.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "Diagnostics/GpuCrash/GpuCrashIncidentSink.h"
#endif

#include "Window/Window.h"
#include "Renderer.h"
#include "Input/InputSystem.h"
#include "World/GameWorld.h"
#include "Input/CameraInputIntentCollector.h"
#include "Level/LevelSession.h"
#include "Time/Timer.h"
#include "Concurrency/ApplicationTaskRuntime.h"

#include <cstdint>
#include <memory>
#include <utility>

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
	if (m_bIsInitialized)
	{
		return;
	}

	InitializeHost();
	InitializeInput();
	InitializeGameWorld();
	InitializeRenderer();
	if (!m_renderer)
	{
		return;
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
	m_levelSession.reset();
	m_cameraInputIntentCollector.reset();
	m_gameWorld.reset();
	m_taskRuntime.reset();
	m_inputSystem.reset();
	m_window.reset();
	m_timer.reset();
	m_bIsInitialized = false;
}
