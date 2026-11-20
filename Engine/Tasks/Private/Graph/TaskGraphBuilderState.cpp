#include "TaskGraphBuilderState.h"

#include <atomic>
#include <utility>

TaskGraphBuilder::State::State(TaskGraphLimits requestedLimits) :
    Limits(requestedLimits),
    BuilderIdentity(AcquireBuilderIdentity())
{
	if (!AreValidLimits(Limits))
	{
		Error = CreateError(TaskGraphErrorCode::InvalidLimits, "Task capacity is zero or a task/edge limit exceeds the hard maximum.");
		return;
	}

	Nodes.reserve(Limits.MaximumTasks);
}

std::uint64_t TaskGraphBuilder::State::AcquireBuilderIdentity() noexcept
{
	static std::atomic_uint64_t nextIdentity{1};
	return nextIdentity.fetch_add(1, std::memory_order_relaxed);
}

bool TaskGraphBuilder::State::AreValidLimits(TaskGraphLimits limits) noexcept
{
	const bool bTaskCountValid = limits.MaximumTasks > 0 && limits.MaximumTasks <= TaskGraphLimits::HardMaximumTasks;
	return bTaskCountValid && limits.MaximumEdges <= TaskGraphLimits::HardMaximumEdges;
}

bool TaskGraphBuilder::State::IsValidTaskLane(TaskLane lane) noexcept
{
	return lane == TaskLane::FrameCritical || lane == TaskLane::Background || lane == TaskLane::BlockingIo;
}

TaskGraphError TaskGraphBuilder::State::CreateError(TaskGraphErrorCode code, std::string message)
{
	return TaskGraphError{.Code = code, .Message = std::move(message)};
}

void TaskGraphBuilder::State::RecordError(TaskGraphErrorCode code, std::string message)
{
	if (!Error)
	{
		Error = CreateError(code, std::move(message));
	}
}

bool TaskGraphBuilder::State::ResolveHandle(TaskNodeHandle handle, std::uint32_t& outIndex)
{
	const std::uint32_t taskCount = static_cast<std::uint32_t>(Nodes.size());
	const bool bHandleValid = TaskGraphAccess::Decode(handle, BuilderIdentity, BuilderGeneration, taskCount, outIndex);
	if (!bHandleValid)
	{
		RecordError(TaskGraphErrorCode::InvalidHandle, "Task handle is invalid, stale, or belongs to another builder.");
		return false;
	}
	return true;
}
