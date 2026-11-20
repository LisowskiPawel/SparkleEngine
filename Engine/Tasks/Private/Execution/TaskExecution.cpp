#include "TaskExecution.h"

#include "Graph/TaskGraphStorage.h"
#include "TaskExecutionState.h"
#include "TaskGraph.h"
#include "TaskTypes.h"
#include "Scheduling/TaskWorkerContext.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>

TaskExecution::State::State()
{
	Data.Status = TaskExecutionStatus::Pending;
}

void TaskExecution::State::RequestCancellation() noexcept
{
	Cancellation.request_stop();
}

void TaskExecution::State::Publish(TaskExecutionCompletion completion)
{
	std::function<void()> onSettled;
	{
		std::scoped_lock lock(Mutex);
		assert(!bSettled && "Task execution completion was published more than once.");
		Data = std::move(completion);
		bSettled = true;
		onSettled = std::move(OnSettled);
	}
	Condition.notify_all();
	if (onSettled)
	{
		onSettled();
	}
}

void TaskExecution::State::WaitUntilSettled()
{
	std::unique_lock lock(Mutex);
	Condition.wait(lock, [this] { return bSettled; });
}

TaskExecution::TaskExecution(std::shared_ptr<State> state) noexcept :
    m_state(std::move(state))
{
}

TaskExecution::TaskExecution() noexcept = default;

TaskExecution::~TaskExecution() = default;

TaskExecution::TaskExecution(TaskExecution&&) noexcept = default;

TaskExecution& TaskExecution::operator=(TaskExecution&&) noexcept = default;

bool TaskExecution::IsValid() const noexcept
{
	return m_state != nullptr;
}

bool TaskExecution::IsSettled() const noexcept
{
	if (m_state == nullptr)
	{
		return false;
	}
	std::scoped_lock lock(m_state->Mutex);
	return m_state->bSettled;
}

bool TaskExecution::WaitFor(std::chrono::milliseconds timeout) const
{
	if (m_state == nullptr || timeout < std::chrono::milliseconds::zero())
	{
		return false;
	}
	const bool bCallerIsWorker = TaskWorkerContext::IsWorkerFor(m_state->ExecutorIdentity);
	const bool bCanWait = !bCallerIsWorker && std::this_thread::get_id() == m_state->JoinThread;
	if (!bCanWait)
	{
		return false;
	}
	std::unique_lock lock(m_state->Mutex);
	return m_state->Condition.wait_for(lock, timeout, [this] { return m_state->bSettled; });
}

TaskExecutionStatus TaskExecution::GetStatus() const noexcept
{
	if (m_state == nullptr)
	{
		return TaskExecutionStatus::Invalid;
	}
	std::scoped_lock lock(m_state->Mutex);
	return m_state->Data.Status;
}

TaskResult TaskExecution::GetResult() const
{
	static const TaskResult invalidResult = TaskResult::Cancelled("Invalid task execution.");
	if (m_state == nullptr)
	{
		return invalidResult;
	}
	std::scoped_lock lock(m_state->Mutex);
	return m_state->Data.Result;
}

std::string TaskExecution::GetFirstFailureTaskName() const
{
	if (m_state == nullptr)
	{
		return {};
	}
	std::scoped_lock lock(m_state->Mutex);
	return m_state->Data.FirstFailureTaskName;
}

std::optional<TaskResult> TaskExecution::GetTaskResult(TaskNodeHandle handle) const
{
	if (m_state == nullptr)
	{
		return std::nullopt;
	}

	std::scoped_lock lock(m_state->Mutex);
	if (!m_state->bSettled)
	{
		return std::nullopt;
	}
	const auto& data = m_state->Data;
	std::uint32_t index = 0;
	const std::uint32_t taskCount = static_cast<std::uint32_t>(data.TaskResults.size());
	const bool bHandleValid = TaskGraphAccess::Decode(handle, data.BuilderIdentity, data.BuilderGeneration, taskCount, index);
	if (!bHandleValid)
	{
		return std::nullopt;
	}
	return data.TaskResults[index];
}
