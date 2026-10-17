#include "TaskDispatch.h"

#include "TaskDispatchState.h"

#include <utility>

TaskDispatch::TaskDispatch() noexcept = default;

TaskDispatch::TaskDispatch(std::shared_ptr<State> state) noexcept :
    m_state(std::move(state)),
    m_execution(m_state->Execution)
{
}

TaskDispatch::~TaskDispatch()
{
	Close();
}

TaskDispatch::TaskDispatch(TaskDispatch&& other) noexcept = default;

TaskDispatch& TaskDispatch::operator=(TaskDispatch&& other) noexcept
{
	if (this != &other)
	{
		Close();
		m_state = std::move(other.m_state);
		m_execution = std::move(other.m_execution);
	}
	return *this;
}

bool TaskDispatch::Notify() noexcept
{
	return m_state != nullptr && m_state->Notify();
}

void TaskDispatch::Close() noexcept
{
	if (m_state != nullptr)
	{
		m_state->Close();
	}
}

TaskDispatch::State::State(std::shared_ptr<TaskExecution::State> execution, TaskDesc desc, TaskFunction function, TaskExecutionContext context) :
    Desc(std::move(desc)),
    Function(std::move(function)),
    Context(std::move(context)),
    Execution(std::move(execution)),
    AllocationFailure(TaskResult::Failure("Prepared task dispatch failed with an exception.")),
    Cancelled(TaskResult::Cancelled("Prepared task dispatch was cancelled."))
{
	Completion.Generation = Execution->Data.Generation;
	Completion.FirstFailureTaskName = Desc.Name.Get();
	TaskExecutionContextBinding::Bind(Context, Completion.Generation, Desc.Lane, Execution->Cancellation.get_token());
	PreparedSettlement = [this]
	{
		ReleaseAdmission();
	};
}

void TaskDispatch::State::CancellationHook::operator()() const noexcept
{
	if (const auto dispatch = Dispatch.lock())
	{
		dispatch->Close();
	}
}

bool TaskDispatch::State::Notify() noexcept
{
	std::lock_guard lock(Gate);
	if (Closed || Owner == nullptr || Execution->Cancellation.stop_requested())
	{
		return false;
	}

	if (CurrentPhase == Phase::Idle)
	{
		CurrentPhase = Phase::Queued;
		Owner->EnqueueDispatch(*this);
	}
	else if (CurrentPhase == Phase::Running)
	{
		Rerun = true;
	}
	return true;
}

void TaskDispatch::State::Close() noexcept
{
	const auto keepAlive = shared_from_this();
	{
		std::lock_guard lock(Gate);
		Closed = true;
		if (Owner == nullptr || CurrentPhase != Phase::Idle)
		{
			return;
		}
		CurrentPhase = Phase::Running;
	}

	Finish(Execution->Cancellation.stop_requested() ? std::move(Cancelled) : TaskResult::Success());
}

void TaskDispatch::State::Execute() noexcept
{
	{
		std::lock_guard lock(Gate);
		CurrentPhase = Phase::Running;
	}

	TaskResult result;
	try
	{
		result = Execution->Cancellation.stop_requested() ? std::move(Cancelled) : Function(Context);
	}
	catch (...)
	{
		result = std::move(AllocationFailure);
	}
	{
		std::lock_guard lock(Gate);
		if (Execution->Cancellation.stop_requested() && result.Succeeded())
		{
			result = std::move(Cancelled);
		}
		if (!result.Succeeded())
		{
			Closed = true;
			Rerun = false;
		}
		if (Rerun)
		{
			Rerun = false;
			CurrentPhase = Phase::Queued;
			Owner->EnqueueDispatch(*this);
			return;
		}
		if (!Closed)
		{
			CurrentPhase = Phase::Idle;
			return;
		}
	}

	Finish(std::move(result));
}

void TaskDispatch::State::Finish(TaskResult result) noexcept
{
	// Captures settle before either scope or executor reports completion.
	Function = {};
	Context = {};

	Completion.Status = result.Failed() ? TaskExecutionStatus::Failed : result.WasCancelled() ? TaskExecutionStatus::Cancelled : TaskExecutionStatus::Succeeded;
	if (!result.Failed())
	{
		Completion.FirstFailureTaskName.clear();
	}
	Completion.Result = std::move(result);
	Completion.SettledTaskCount = 1;
	Execution->Publish(std::move(Completion));
}

void TaskDispatch::State::ReleaseAdmission() noexcept
{
	const auto keepAlive = shared_from_this();
	{
		std::lock_guard lock(Gate);
		auto* owner = Owner;
		Owner = nullptr;
		owner->ReleaseDispatch(*this);
	}

	auto scopeSettled = std::move(ScopeSettled);
	if (scopeSettled)
	{
		scopeSettled();
	}
}
