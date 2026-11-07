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
	constexpr void Set(ERhiQueueType queue, bool bSupported, bool bIndependent) noexcept
	{
		if (!IsRhiQueueTypeValid(queue))
		{
			return;
		}

		Entries[RhiQueueTypeToIndex(queue)] = RhiQueueCapability{.bSupported = bSupported, .bIndependent = bSupported && bIndependent};
	}

	constexpr RhiQueueCapability Get(ERhiQueueType queue) const noexcept { return IsRhiQueueTypeValid(queue) ? Entries[RhiQueueTypeToIndex(queue)] : RhiQueueCapability{}; }

	constexpr bool Supports(ERhiQueueType queue) const noexcept { return Get(queue).bSupported; }

	constexpr bool IsIndependent(ERhiQueueType queue) const noexcept { return Get(queue).bIndependent; }

	constexpr bool SupportsIndependent(ERhiQueueType queue) const noexcept
	{
		const RhiQueueCapability capability = Get(queue);
		return capability.bSupported && capability.bIndependent;
	}

	std::array<RhiQueueCapability, RhiQueueTypeCount> Entries{};
};
