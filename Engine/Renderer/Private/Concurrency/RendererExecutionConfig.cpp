#include "PCH.h"

#include "Renderer/Public/Concurrency/RendererExecutionConfig.h"
#include "Core/Public/Diagnostics/Error.h"
#include "RHI/Public/CVars/RHICVars.h"
#include <TaskScheduler_c.h>

bool RendererExecutionConfig::IsThreaded() const noexcept
{
	return Mode == RendererExecutionMode::Threaded;
}

void RendererExecutionConfig::Validate() const
{
	const bool bAnyNative = FrameScheduler || IoScheduler;
	const bool bSomeNative = bAnyNative || BackgroundScheduler;
	const bool bMissingFirst = !FrameScheduler || !IoScheduler;
	const bool bMissingResource = bMissingFirst || !BackgroundScheduler;
	if (bSomeNative && bMissingResource)
	{
		throw Diagnostics::Error("Renderer requires all native frame/CPU/I/O resources, or none.");
	}

	if (bSomeNative)
	{
		const bool bFrameAliased = FrameScheduler == IoScheduler || FrameScheduler == BackgroundScheduler;
		if (bFrameAliased || IoScheduler == BackgroundScheduler)
		{
			throw Diagnostics::Error("Renderer frame, CPU and I/O resources must have independent physical workers.");
		}
		const auto frame = enkiGetTaskSchedulerConfig(FrameScheduler);
		const auto cpu = enkiGetTaskSchedulerConfig(BackgroundScheduler);
		const auto io = enkiGetTaskSchedulerConfig(IoScheduler);
		const bool bSomeWorkersMissing = frame.numTaskThreadsToCreate == 0 || cpu.numTaskThreadsToCreate == 0;
		const bool bAnyWorkersMissing = bSomeWorkersMissing || io.numTaskThreadsToCreate == 0;
		const std::uint64_t workers = static_cast<std::uint64_t>(frame.numTaskThreadsToCreate) + cpu.numTaskThreadsToCreate + io.numTaskThreadsToCreate;
		if (bAnyWorkersMissing || workers > 256)
		{
			throw Diagnostics::Error("Renderer native resources require positive capacities with at most 256 physical workers in total.");
		}
		const bool bSomeSlotsMissing = frame.numExternalTaskThreads == 0 || cpu.numExternalTaskThreads == 0;
		const bool bAnySlotsMissing = bSomeSlotsMissing || io.numExternalTaskThreads == 0;
		if (IsThreaded() && bAnySlotsMissing)
		{
			throw Diagnostics::Error("Threaded Renderer requires an external registration slot in each native resource.");
		}
	}
	if (Mode != RendererExecutionMode::Serial && Mode != RendererExecutionMode::Threaded)
	{
		throw Diagnostics::Error("RendererExecutionConfig has an unknown execution mode.");
	}
	if (!IsThreaded())
	{
		return;
	}
	if (RenderPipelineDepth > 2u)
	{
		throw Diagnostics::Error("r.RenderPipelineDepth is outside the supported range [0, 2].");
	}
	if (RenderPipelineDepth >= CVarMaximumFramesInFlight.Get())
	{
		throw Diagnostics::Error("r.RenderPipelineDepth must be lower than r.MaximumFramesInFlight so queued simulation frames have distinct GPU frame slots.");
	}
}

std::uint32_t RendererExecutionConfig::GetFrameQueueCapacity() const noexcept
{
	return IsThreaded() ? RenderPipelineDepth + 1u : 1u;
}
