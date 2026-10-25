#include "PCH.h"
#include "View/RenderViewPreparation.h"

#include "Tasks/Public/ParallelFor.h"

#include <algorithm>
#include <bit>
#include <limits>

void RenderViewPreparation::EnsureGraph(std::size_t primitiveCount)
{
	const std::size_t maximumCapacity = (std::numeric_limits<std::uint32_t>::max)();
	const std::uint32_t boundedCount = static_cast<std::uint32_t>((std::min) (primitiveCount, maximumCapacity));
	const std::uint32_t capacity = boundedCount <= 128u ? 128u : std::bit_ceil(boundedCount);
	if (m_graph && capacity == m_capacity)
	{
		return;
	}

	TaskGraphBuilder builder;
	(void) ParallelFor(
	    builder,
	    TaskDesc{.Name = TaskName("Renderer.View.Visibility"), .Lane = TaskLane::FrameCritical},
	    capacity,
	    ParallelForPolicy{.GrainSize = 64u, .SerialThreshold = 128u, .MaximumPartitions = 8u},
	    &EvaluateRenderViewVisibility);
	m_graph = builder.Compile();
	m_capacity = capacity;
}
