#pragma once

#include "RendererAPI.h"

#include <cstdint>

struct enkiTaskScheduler;

enum class RendererExecutionMode : std::uint8_t
{
	Serial = 0,
	Threaded = 1,
};

struct SPARKLE_RENDERER_API RendererExecutionConfig final
{
	RendererExecutionMode Mode = RendererExecutionMode::Serial;
	std::uint32_t RenderPipelineDepth = 0;
	bool bEnableUiRenderPackets = false;
	// Borrowed from the application host; null selects direct frame kernels.
	enkiTaskScheduler* FrameScheduler = nullptr;
	enkiTaskScheduler* IoScheduler = nullptr;
	enkiTaskScheduler* BackgroundScheduler = nullptr;

	bool IsThreaded() const noexcept;
	void Validate() const;
	std::uint32_t GetFrameQueueCapacity() const noexcept;
};
