#include "PCH.h"

#include "Concurrency/ApplicationTaskRuntime.h"

#include "Core/Public/Console/CVar.h"
#include "Tasks/Public/TaskExecutor.h"
#include "Tasks/Public/TaskScope.h"

#include <chrono>

namespace TaskRuntimeCVars
{
	static ConsoleVariable<std::uint32_t> g_frameCriticalWorkerCount{"task.FrameCriticalWorkerCount", 1, "Frame-critical task workers. Sweep explicitly before changing the product default."};
	static ConsoleVariable<std::uint32_t> g_backgroundWorkerCount{"task.BackgroundWorkerCount", 1, "Background task workers. Sweep independently from frame-critical work."};
	static ConsoleVariable<std::uint32_t> g_blockingIoWorkerCount{"task.BlockingIoWorkerCount", 1, "Blocking-I/O task workers. Keep bounded independently from CPU task lanes."};
	static ConsoleVariable<bool> g_bSerialExecution{"task.SerialExecution", false, "Run SparkleTasks on the deterministic caller-thread reference executor."};
}

TaskExecutorConfig ApplicationTaskRuntime::BuildExecutorConfig() noexcept
{
	if (TaskRuntimeCVars::g_bSerialExecution.Get())
	{
		return {};
	}

	return TaskExecutorConfig{
	    .FrameCriticalWorkerCount = TaskRuntimeCVars::g_frameCriticalWorkerCount.Get(),
	    .BackgroundWorkerCount = TaskRuntimeCVars::g_backgroundWorkerCount.Get(),
	    .BlockingIoWorkerCount = TaskRuntimeCVars::g_blockingIoWorkerCount.Get(),
	    .MaximumTasksPerExecution = 1'024,
	    .MaximumEdgesPerExecution = 4'096,
	    .MaximumActiveExecutions = 64};
}

ApplicationTaskRuntime::ApplicationTaskRuntime() :
    m_executor(std::make_unique<TaskExecutor>(BuildExecutorConfig())),
    m_applicationScope(std::make_unique<TaskScope>(TaskScopeDesc{TaskScopeKind::Application, "Application"}))
{
}

ApplicationTaskRuntime::~ApplicationTaskRuntime()
{
	m_applicationScope->Cancel();
	m_executor->Shutdown(TaskExecutorShutdownMode::Cancel);
	m_applicationScope->JoinFor(std::chrono::milliseconds::zero());
}

TaskExecutor& ApplicationTaskRuntime::GetExecutor() noexcept
{
	return *m_executor;
}

TaskScope& ApplicationTaskRuntime::GetApplicationScope() noexcept
{
	return *m_applicationScope;
}
