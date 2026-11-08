#include "TaskExecutorImplementation.h"

#include "TaskDispatchState.h"
#include "TaskWorkerContext.h"

#include <utility>

bool TaskExecutor::Implementation::Shutdown(TaskExecutorShutdownMode mode) noexcept
{
	if (TaskWorkerContext::IsWorkerFor(this))
	{
		return false;
	}

	std::unique_lock shutdownLock(m_shutdownMutex);
	const std::vector<std::shared_ptr<TaskExecution::State>> executionsToCancel = BeginShutdown(mode);
	RequestCancellation(executionsToCancel);
	CloseDispatches();
	WaitForActiveExecutions();
	FinishShutdown();
	return true;
}

std::vector<std::shared_ptr<TaskExecution::State>> TaskExecutor::Implementation::BeginShutdown(TaskExecutorShutdownMode mode)
{
	std::vector<std::shared_ptr<TaskExecution::State>> executionsToCancel;
	std::unique_lock lock(m_stateMutex);
	if (m_lifecycle == LifecycleState::Stopped)
	{
		return executionsToCancel;
	}
	if (m_lifecycle == LifecycleState::Accepting)
	{
		m_lifecycle = mode == TaskExecutorShutdownMode::Cancel ? LifecycleState::Cancelling : LifecycleState::Draining;
	}
	if (m_lifecycle == LifecycleState::Cancelling)
	{
		for (auto& item : m_executions)
		{
			if (auto execution = item.lock())
			{
				executionsToCancel.push_back(std::move(execution));
			}
		}
	}

	return executionsToCancel;
}

void TaskExecutor::Implementation::RequestCancellation(std::span<const std::shared_ptr<TaskExecution::State>> executions) noexcept
{
	for (const auto& execution : executions)
	{
		execution->RequestCancellation();
	}
}

void TaskExecutor::Implementation::WaitForActiveExecutions()
{
	std::unique_lock lock(m_stateMutex);
	m_stateCondition.wait(lock, [this] { return m_activeExecutions == 0; });
	m_lifecycle = LifecycleState::Stopping;
}

void TaskExecutor::Implementation::FinishShutdown()
{
	RequestWorkerStop();
	JoinWorkers();

	{
		std::lock_guard lock(m_stateMutex);
		m_lifecycle = LifecycleState::Stopped;
		m_executions.clear();
	}

	m_stateCondition.notify_all();
}

void TaskExecutor::Implementation::CloseDispatches() noexcept
{
	for (std::size_t index = 0;; ++index)
	{
		std::shared_ptr<TaskDispatch::State> dispatch;
		{
			std::lock_guard lock(m_stateMutex);
			if (index >= m_dispatches.size())
			{
				return;
			}
			dispatch = m_dispatches[index];
		}
		if (dispatch)
		{
			dispatch->Close();
		}
	}
}
