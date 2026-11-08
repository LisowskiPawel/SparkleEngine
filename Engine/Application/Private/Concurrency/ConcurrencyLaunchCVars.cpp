#include "PCH.h"

#include "Concurrency/ConcurrencyLaunchCVars.h"

#include "Core/Public/Console/CVar.h"
#include "Renderer/Public/Concurrency/RendererExecutionConfig.h"

namespace ConcurrencyLaunchCVars
{
	ConsoleVariable<bool> g_threadedRenderer("r.ThreadedRenderer", true, "Run renderer/RHI ownership on Sparkle.RenderThread.");
	ConsoleVariable<std::uint32_t> g_renderPipelineDepth("r.RenderPipelineDepth", 1, "Bounded renderer CPU lead in frames. Supported values are 0 through 2.");

	void Register() noexcept
	{
	}

	RendererExecutionConfig ResolveRendererExecution(bool bAllowThreaded, bool bEnableUiRenderPackets, TaskExecutor& executor, TaskScope& parentScope) noexcept
	{
		RendererExecutionConfig config;
		if (bAllowThreaded && g_threadedRenderer.Get())
		{
			config.Mode = RendererExecutionMode::Threaded;
			config.RenderPipelineDepth = g_renderPipelineDepth.Get();
		}
		config.bEnableUiRenderPackets = bEnableUiRenderPackets;
		config.AssetTaskExecutor = &executor;
		config.AssetTaskParentScope = &parentScope;
		return config;
	}
}
