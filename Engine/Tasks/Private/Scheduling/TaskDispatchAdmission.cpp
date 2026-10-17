#include "TaskDispatchState.h"

#include "TaskWorkerContext.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>

std::shared_ptr<TaskDispatch::State> TaskExecutor::Implementation::Runtime::PrepareDispatch(
    const std::shared_ptr<TaskScope::State>& scope,
    TaskDesc desc,
    TaskFunction function,
    TaskExecutionContext context)
{
	if (TaskWorkerContext::IsWorker() || std::this_thread::get_id() != scope->OwnerThread || !desc.Name.IsValid() || !function || desc.CompletionPolicy != TaskCompletionPolicy::Normal
	    || GetWorkerCount(desc.Lane) == 0 || (context.HasUserData() && !context.HasOwnedUserData()))
	{
		throw std::invalid_argument("Prepared dispatch requires its scope owner, a callable, owned context and an asynchronous Normal lane.");
	}

	const std::uint64_t generation = m_nextExecutionGeneration.fetch_add(1, std::memory_order_relaxed);
	auto execution = CreateExecution(generation, scope);
	auto dispatch = std::make_shared<TaskDispatch::State>(execution, std::move(desc), std::move(function), std::move(context));
	dispatch->Cancellation.emplace(execution->Cancellation.get_token(), TaskDispatch::State::CancellationHook{dispatch});
	std::lock_guard gate(dispatch->Gate);
	std::lock_guard lifecycle(m_stateMutex);
	if (m_lifecycle != LifecycleState::Accepting || m_activeExecutions >= m_config.MaximumActiveExecutions)
	{
		throw std::runtime_error("Task executor cannot admit a prepared dispatch.");
	}
	if (m_dispatches.empty())
	{
		m_dispatches.resize(m_config.MaximumActiveExecutions);
	}
	const auto available = std::ranges::find(m_dispatches, nullptr);
	assert(available != m_dispatches.end());

	std::erase_if(
	    m_executions,
	    [](const auto& item)
	    {
		    const auto state = item.lock();
		    if (!state)
		    {
			    return true;
		    }
		    std::lock_guard lock(state->Mutex);
		    return state->Settled;
	    });

	// Reserve before scope admission. Nothing after RegisterExecution can allocate.
	m_executions.reserve(m_executions.size() + 1);
	if (!scope->RegisterExecution(execution))
	{
		throw std::runtime_error("Prepared dispatch scope is closed or settled.");
	}

	dispatch->ScopeSettled = std::move(execution->OnSettled);
	execution->OnSettled = std::move(dispatch->PreparedSettlement);
	dispatch->Slot = static_cast<std::size_t>(available - m_dispatches.begin());
	*available = dispatch;
	m_executions.emplace_back(execution);
	++m_activeExecutions;
	dispatch->Owner = this;
	return dispatch;
}

std::shared_ptr<TaskDispatch::State> TaskExecutor::Implementation::PrepareDispatch(TaskScope& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context)
{
	return m_runtime->PrepareDispatch(scope.m_state, std::move(desc), std::move(function), std::move(context));
}

void TaskExecutor::Implementation::Runtime::EnqueueDispatch(TaskDispatch::State& dispatch) noexcept
{
	TaskLaneState& lane = m_lanes[LaneIndex(dispatch.Desc.Lane)];
	{
		std::lock_guard lock(lane.InjectionMutex);
		dispatch.ReadyNext = nullptr;
		if (lane.DispatchTail != nullptr)
		{
			lane.DispatchTail->ReadyNext = &dispatch;
		}
		else
		{
			lane.DispatchHead = &dispatch;
		}
		lane.DispatchTail = &dispatch;
	}
	{
		std::lock_guard lock(lane.WakeMutex);
		++lane.WakeEpoch;
	}
	lane.WakeCondition.notify_one();
}

bool TaskExecutor::Implementation::Runtime::TryPopDispatch(TaskLane laneType, ReadyTask& task)
{
	TaskLaneState& lane = m_lanes[LaneIndex(laneType)];
	std::lock_guard lock(lane.InjectionMutex);
	auto* dispatch = lane.DispatchHead;
	if (dispatch == nullptr)
	{
		return false;
	}
	lane.DispatchHead = dispatch->ReadyNext;
	if (lane.DispatchHead == nullptr)
	{
		lane.DispatchTail = nullptr;
	}
	dispatch->ReadyNext = nullptr;
	task.Dispatch = dispatch->shared_from_this();
	return true;
}

void TaskExecutor::Implementation::Runtime::ReleaseDispatch(TaskDispatch::State& dispatch) noexcept
{
	std::lock_guard lock(m_stateMutex);
	m_dispatches[dispatch.Slot].reset();
	assert(m_activeExecutions != 0);
	--m_activeExecutions;
	m_stateCondition.notify_all();
}

void TaskExecutor::Implementation::Runtime::CloseDispatches() noexcept
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
