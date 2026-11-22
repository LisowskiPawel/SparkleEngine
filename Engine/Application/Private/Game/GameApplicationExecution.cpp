#include "PCH.h"
#include "Game/GameApplication.h"

#include "Core/Public/Console/CVar.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Threading/ThreadOwnership.h"
#include <TaskScheduler_c.h>
#include <new>

namespace TaskRuntimeCVars
{
	static ConsoleVariable<std::uint32_t> g_frameCriticalWorkerCount{"task.FrameCriticalWorkerCount", 1, "Frame-critical task workers. Sweep explicitly before changing the product default."};
	static ConsoleVariable<std::uint32_t> g_backgroundWorkerCount{"task.BackgroundWorkerCount", 1, "Background task workers. Sweep independently from frame-critical work."};
	static ConsoleVariable<std::uint32_t> g_blockingIoWorkerCount{"task.BlockingIoWorkerCount", 1, "Blocking-I/O task workers. Keep bounded independently from CPU task lanes."};
	static ConsoleVariable<bool> g_bSerialExecution{"task.SerialExecution", false, "Run task kernels directly on their deterministic caller-thread path."};
}

void GameApplication::InitializeExecution()
{
	m_stop = std::stop_source{};
	const bool bSerial = TaskRuntimeCVars::g_bSerialExecution.Get();
	const auto frameWorkers = bSerial ? 0u : TaskRuntimeCVars::g_frameCriticalWorkerCount.Get();
	const auto backgroundWorkers = bSerial ? 0u : TaskRuntimeCVars::g_backgroundWorkerCount.Get();
	const auto ioWorkers = bSerial ? 0u : TaskRuntimeCVars::g_blockingIoWorkerCount.Get();
	const std::uint64_t totalWorkers = static_cast<std::uint64_t>(frameWorkers) + backgroundWorkers + ioWorkers;
	const bool bSomeDisabled = frameWorkers == 0 || backgroundWorkers == 0;
	const bool bAnyDisabled = bSomeDisabled || ioWorkers == 0;
	const bool bMixedCounts = totalWorkers != 0 && bAnyDisabled;
	if (totalWorkers > 256 || bMixedCounts)
	{
		throw Diagnostics::Error("Native task capacities require all zero or positive frame/background/I/O counts, with at most 256 workers in total.");
	}
	m_frameScheduler = CreateExecutionScheduler(frameWorkers, [](std::uint32_t) { Threading::SetCurrentThreadRole("Sparkle.Task.FrameCritical"); });
	m_backgroundScheduler = CreateExecutionScheduler(backgroundWorkers, [](std::uint32_t) { Threading::SetCurrentThreadRole("Sparkle.Task.Background"); });
	m_ioScheduler = CreateExecutionScheduler(ioWorkers, [](std::uint32_t) { Threading::SetCurrentThreadRole("Sparkle.Task.BlockingIo"); });
}

enkiTaskScheduler* GameApplication::CreateExecutionScheduler(std::uint32_t workers, void (*threadStart)(std::uint32_t))
{
	if (workers == 0)
	{
		return nullptr;
	}
	auto* scheduler = enkiNewTaskScheduler();
	if (!scheduler)
	{
		throw std::bad_alloc();
	}
	auto config = enkiGetTaskSchedulerConfig(scheduler);
	config.numTaskThreadsToCreate = workers;
	config.numExternalTaskThreads = 1;
	config.profilerCallbacks.threadStart = threadStart;
	char error[256]{};
	if (!enkiTryInitTaskSchedulerWithConfig(scheduler, config, error, sizeof(error)))
	{
		enkiDeleteTaskScheduler(scheduler);
		throw Diagnostics::Error(error);
	}
	return scheduler;
}

void GameApplication::ShutdownExecution() noexcept
{
	for (auto* scheduler : {&m_frameScheduler, &m_backgroundScheduler, &m_ioScheduler})
	{
		if (*scheduler)
		{
			enkiWaitforAllAndShutdown(*scheduler);
			enkiDeleteTaskScheduler(*scheduler);
			*scheduler = nullptr;
		}
	}
}
