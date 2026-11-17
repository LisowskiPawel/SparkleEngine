#pragma once

#include "RhiQueue.h"

#include <array>

struct RhiQueueCapability final
{
	bool bSupported = false;
	bool bIndependent = false;
};

struct RhiQueueCapabilities final
{
	SPARKLE_RHI_API void Set(ERhiQueueType queue, bool bSupported, bool bIndependent) noexcept;

	SPARKLE_RHI_API RhiQueueCapability Get(ERhiQueueType queue) const noexcept;

	SPARKLE_RHI_API bool Supports(ERhiQueueType queue) const noexcept;

	SPARKLE_RHI_API bool IsIndependent(ERhiQueueType queue) const noexcept;

	SPARKLE_RHI_API bool SupportsIndependent(ERhiQueueType queue) const noexcept;

	std::array<RhiQueueCapability, RhiQueueTypeCount> Entries{};
};
