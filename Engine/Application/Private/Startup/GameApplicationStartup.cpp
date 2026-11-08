#include "PCH.h"
#include "GameApplication.h"

#include "Concurrency/ApplicationTaskRuntime.h"
#include "Concurrency/ConcurrencyLaunchCVars.h"
#include "Input/CameraInputIntentCollector.h"
#include "Input/InputSystem.h"
#include "Level/LevelSession.h"
#include "Renderer.h"
#include "Renderer/Public/Concurrency/RendererExecutionConfig.h"
#include "Startup/ApplicationGraphicsLaunch.h"
#include "Time/Timer.h"
#include "Window/Window.h"
#include "World/GameWorld.h"

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
	m_taskRuntime = std::make_unique<ApplicationTaskRuntime>();
	m_gameWorld = std::make_unique<GameWorld>(m_taskRuntime->GetExecutor());
	m_cameraInputIntentCollector = std::make_unique<CameraInputIntentCollector>(*m_inputSystem, *m_window);

	m_levelSession = std::make_unique<LevelSession>(*m_gameWorld, m_taskRuntime->GetExecutor(), m_taskRuntime->GetApplicationScope());
	m_levelSession->GetLevelChangeEvents().OnLevelChanged.Add([this](const LevelChangedEventArgs& event) { SignalLevelReadiness(event.activeLevelName); });
}

void GameApplication::InitializeRenderer()
{
	const RendererExecutionConfig execution = ConcurrencyLaunchCVars::ResolveRendererExecution(
	    m_options.bAllowThreadedRenderer,
	    m_options.bEnableUiRenderPackets,
	    m_taskRuntime->GetExecutor(),
	    m_taskRuntime->GetApplicationScope());

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
