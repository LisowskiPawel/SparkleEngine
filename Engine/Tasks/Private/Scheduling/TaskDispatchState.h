#pragma once

#include "TaskDispatch.h"
#include "TaskExecutorRuntime.h"

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>

struct TaskDispatch::State final : std::enable_shared_from_this<TaskDispatch::State>
{
	enum class Phase : std::uint8_t
	{
		Idle,
		Queued,
		Running
	};

	struct CancellationHook final
	{
		std::weak_ptr<State> Dispatch;
		void operator()() const noexcept;
	};

	State(std::shared_ptr<TaskExecution::State> execution, TaskDesc desc, TaskFunction function, TaskExecutionContext context);
	bool Notify() noexcept;
	void Close() noexcept;
	void Execute() noexcept;
	void Finish(TaskResult result) noexcept;
	void ReleaseAdmission() noexcept;

	// Gate -> lane injection -> lane wake. Workers unlink before acquiring Gate.
	std::mutex Gate;
	TaskExecutor::Implementation::Runtime* Owner = nullptr;
	TaskDesc Desc;
	TaskFunction Function;
	TaskExecutionContext Context;
	std::shared_ptr<TaskExecution::State> Execution;
	TaskExecutionCompletion Completion;
	TaskResult AllocationFailure;
	TaskResult Cancelled;
	std::function<void()> ScopeSettled;
	std::function<void()> PreparedSettlement;
	std::optional<std::stop_callback<CancellationHook>> Cancellation;
	State* ReadyNext = nullptr;
	std::size_t Slot = 0;
	Phase CurrentPhase = Phase::Idle;
	bool Closed = false;
	bool Rerun = false;
};
