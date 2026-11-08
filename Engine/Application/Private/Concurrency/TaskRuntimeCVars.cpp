#include "PCH.h"

#include "Concurrency/TaskRuntimeCVars.h"

#include "Core/Public/Console/CVar.h"

namespace TaskRuntimeCVars
{
	ConsoleVariable<std::uint32_t> g_frameCriticalWorkerCount{"task.FrameCriticalWorkerCount", 1, "Frame-critical task workers. Sweep explicitly before changing the product default."};

	ConsoleVariable<std::uint32_t> g_backgroundWorkerCount{"task.BackgroundWorkerCount", 1, "Background task workers. Sweep independently from frame-critical work."};
	ConsoleVariable<std::uint32_t> g_blockingIoWorkerCount{"task.BlockingIoWorkerCount", 1, "Blocking-I/O task workers. Keep bounded independently from CPU task lanes."};
	ConsoleVariable<bool> g_serialExecution{"task.SerialExecution", false, "Run SparkleTasks on the deterministic caller-thread reference executor."};
}

namespace TaskRuntimeCVars
{
	void Register() noexcept
	{
	}

	std::uint32_t ResolveFrameCriticalWorkerCount() noexcept
	{
		return g_frameCriticalWorkerCount.Get();
	}

	std::uint32_t ResolveBackgroundWorkerCount() noexcept
	{
		return g_backgroundWorkerCount.Get();
	}

	std::uint32_t ResolveBlockingIoWorkerCount() noexcept
	{
		return g_blockingIoWorkerCount.Get();
	}

	bool UseSerialExecution() noexcept
	{
		return g_serialExecution.Get();
	}
}
