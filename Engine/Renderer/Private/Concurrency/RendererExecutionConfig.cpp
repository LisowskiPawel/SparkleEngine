#include "PCH.h"

#include "Renderer/Public/Concurrency/RendererExecutionConfig.h"
#include "Core/Public/Diagnostics/Error.h"
#include "RHI/Public/CVars/RHICVars.h"

bool RendererExecutionConfig::IsThreaded() const noexcept
{
	return Mode == RendererExecutionMode::Threaded;
}

bool RendererExecutionConfig::HasAssetTaskRuntime() const noexcept
{
	return AssetTaskExecutor != nullptr && AssetTaskParentScope != nullptr;
}

void RendererExecutionConfig::Validate() const
{
	if (!HasAssetTaskRuntime())
	{
		throw Diagnostics::Error("RendererExecutionConfig has no asset-task executor or parent scope.");
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
