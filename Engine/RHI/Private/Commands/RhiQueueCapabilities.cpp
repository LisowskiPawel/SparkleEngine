#include "PCH.h"

#include "Commands/RhiQueueCapabilities.h"

void RhiQueueCapabilities::Set(ERhiQueueType queue, bool bSupported, bool bIndependent) noexcept
{
	if (!IsRhiQueueTypeValid(queue))
	{
		return;
	}

	Entries[RhiQueueTypeToIndex(queue)] = RhiQueueCapability{.bSupported = bSupported, .bIndependent = bSupported && bIndependent};
}

RhiQueueCapability RhiQueueCapabilities::Get(ERhiQueueType queue) const noexcept
{
	return IsRhiQueueTypeValid(queue) ? Entries[RhiQueueTypeToIndex(queue)] : RhiQueueCapability{};
}

bool RhiQueueCapabilities::Supports(ERhiQueueType queue) const noexcept
{
	return Get(queue).bSupported;
}

bool RhiQueueCapabilities::IsIndependent(ERhiQueueType queue) const noexcept
{
	return Get(queue).bIndependent;
}

bool RhiQueueCapabilities::SupportsIndependent(ERhiQueueType queue) const noexcept
{
	const RhiQueueCapability capability = Get(queue);
	return capability.bSupported && capability.bIndependent;
}
