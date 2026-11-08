#include "TaskDispatchState.h"

#include "TaskWorkerContext.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>

std::shared_ptr<TaskDispatch::State> TaskExecutor::Implementation::PrepareDispatch(TaskScope& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context)
{
	ValidateDispatchRequest(*scope.m_state, desc, function, context);
	auto dispatch = CreateDispatch(scope.m_state, std::move(desc), std::move(function), std::move(context));
	AdmitDispatch(scope.m_state, dispatch);
	return dispatch;
}

void TaskExecutor::Implementation::ValidateDispatchRequest(const TaskScope::State& scope, const TaskDesc& desc, const TaskFunction& function, const TaskExecutionContext& context) const
{
	const bool bCallerIsWorker = TaskWorkerContext::IsWorker();
	const bool bOwnerThreadMatches = std::this_thread::get_id() == scope.OwnerThread;
	if (bCallerIsWorker || !bOwnerThreadMatches)
	{
		throw std::invalid_argument("Prepared dispatch requires its scope owner outside executor workers.");
	}

	const bool bDescriptionValid = desc.Name.IsValid() && function;
	const bool bNormalCompletion = desc.CompletionPolicy == TaskCompletionPolicy::Normal;
	if (!bDescriptionValid || !bNormalCompletion)
	{
		throw std::invalid_argument("Prepared dispatch requires a named callable with Normal completion policy.");
	}

	const bool bLaneAvailable = GetWorkerCount(desc.Lane) != 0;
	const bool bBorrowedUserData = context.HasUserData() && !context.HasOwnedUserData();
	if (!bLaneAvailable || bBorrowedUserData)
	{
		throw std::invalid_argument("Prepared dispatch requires owned context and a configured asynchronous lane.");
	}
}

std::shared_ptr<TaskDispatch::State> TaskExecutor::Implementation::CreateDispatch(const std::shared_ptr<TaskScope::State>& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context)
{
	const std::uint64_t generation = m_nextExecutionGeneration.fetch_add(1, std::memory_order_relaxed);
	auto execution = CreateExecution(generation, scope);
	auto dispatch = std::make_shared<TaskDispatch::State>(std::move(execution), std::move(desc), std::move(function), std::move(context));
	dispatch->Cancellation.emplace(dispatch->Execution->Cancellation.get_token(), TaskDispatch::State::CancellationHook{dispatch});
	return dispatch;
}

void TaskExecutor::Implementation::AdmitDispatch(const std::shared_ptr<TaskScope::State>& scope, const std::shared_ptr<TaskDispatch::State>& dispatch)
{
	std::lock_guard gate(dispatch->Gate);
	std::lock_guard lifecycle(m_stateMutex);
	const std::size_t slot = ReserveDispatchSlot();

	// Scope registration is the ownership commit. Every later operation is non-allocating.
	const bool bScopeRegistered = scope->RegisterExecution(dispatch->Execution);
	if (!bScopeRegistered)
	{
		throw std::runtime_error("Prepared dispatch scope is closed or settled.");
	}

	CommitDispatchAdmission(dispatch, slot);
}

std::size_t TaskExecutor::Implementation::ReserveDispatchSlot()
{
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
	ReserveExecutionStorage();
	return static_cast<std::size_t>(available - m_dispatches.begin());
}

void TaskExecutor::Implementation::CommitDispatchAdmission(const std::shared_ptr<TaskDispatch::State>& dispatch, std::size_t slot) noexcept
{
	const auto& execution = dispatch->Execution;
	dispatch->ScopeSettled = std::move(execution->OnSettled);
	execution->OnSettled = std::move(dispatch->PreparedSettlement);
	dispatch->Slot = slot;
	m_dispatches[slot] = dispatch;
	m_executions.emplace_back(execution);
	++m_activeExecutions;
	dispatch->Owner = this;
}

void TaskExecutor::Implementation::ReleaseDispatch(TaskDispatch::State& dispatch) noexcept
{
	std::lock_guard lock(m_stateMutex);
	m_dispatches[dispatch.Slot].reset();
	assert(m_activeExecutions != 0);
	--m_activeExecutions;
	m_stateCondition.notify_all();
}
