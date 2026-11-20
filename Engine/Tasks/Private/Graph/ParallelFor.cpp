#include "ParallelFor.h"

#include "TaskExecutionContext.h"
#include "TaskGraphStorage.h"

#include <algorithm>
#include <utility>

TaskNodeHandle ParallelFor(TaskGraphBuilder& graph, TaskDesc desc, std::uint32_t itemCount, ParallelForPolicy policy, ParallelForFunction function)
{
	if (policy.GrainSize == 0 || policy.MaximumPartitions == 0)
	{
		TaskGraphAccess::RecordError(graph, TaskGraphErrorCode::InvalidParallelForPolicy, "ParallelFor grain size and maximum partitions must be non-zero.");
		return {};
	}

	if (itemCount <= policy.SerialThreshold || itemCount <= policy.GrainSize)
	{
		auto serialTask = [itemCount, function = std::move(function)](TaskExecutionContext& context)
		{
			return function ? function(0, itemCount, context) : TaskResult::Success();
		};

		return graph.Add(std::move(desc), std::move(serialTask));
	}

	TaskDesc groupDesc = desc;
	groupDesc.Name = desc.Name + ".Group";
	const TaskNodeHandle group = graph.Add(std::move(groupDesc), [](TaskExecutionContext&) { return TaskResult::Success(); });
	if (!group)
	{
		return {};
	}

	const auto divideRoundUp = [](std::uint32_t value, std::uint32_t divisor)
	{
		return static_cast<std::uint32_t>((static_cast<std::uint64_t>(value) + divisor - 1u) / divisor);
	};

	const std::uint32_t grainPartitionCount = divideRoundUp(itemCount, policy.GrainSize);
	const std::uint32_t partitionCount = std::min(policy.MaximumPartitions, grainPartitionCount);
	for (std::uint32_t partition = 0; partition < partitionCount; ++partition)
	{
		const std::uint32_t begin = static_cast<std::uint32_t>(static_cast<std::uint64_t>(itemCount) * partition / partitionCount);
		const std::uint32_t end = static_cast<std::uint32_t>(static_cast<std::uint64_t>(itemCount) * (partition + 1u) / partitionCount);
		TaskDesc partitionDesc = desc;
		partitionDesc.Name = desc.Name + ".Range" + std::to_string(partition);

		auto partitionTask = [begin, end, function](TaskExecutionContext& context)
		{
			return function ? function(begin, end, context) : TaskResult::Success();
		};

		const TaskNodeHandle partitionHandle = graph.AddNested(group, std::move(partitionDesc), std::move(partitionTask));

		if (!partitionHandle)
		{
			return {};
		}
	}
	return group;
}
