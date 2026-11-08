#pragma once

#include "Execution/TaskExecutionState.h"
#include "Lifetime/TaskScopeState.h"
#include "TaskExecutor.h"

#include <cstddef>
#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <span>
#include <string_view>
#include <thread>
#include <vector>

struct TaskExecutor::Implementation final
{
	static constexpr std::size_t TaskLaneCount = 3;

	class ScheduledTaskExecution;

	struct ReadyTask final
	{
		std::shared_ptr<ScheduledTaskExecution> Execution;
		std::uint32_t TaskIndex = 0;
		std::shared_ptr<TaskDispatch::State> Dispatch;
	};

	struct alignas(64) TaskWorker final
	{
		TaskLane Lane = TaskLane::FrameCritical;
		std::uint32_t LaneWorkerIndex = 0;
		std::mutex QueueMutex;
		std::deque<ReadyTask> ReadyQueue;
		std::thread Thread;
		bool bPreferDispatch = true;
	};

	struct TaskLaneState final
	{
		std::mutex InjectionMutex;
		std::deque<ReadyTask> InjectionQueue;
		TaskDispatch::State* DispatchHead = nullptr;
		TaskDispatch::State* DispatchTail = nullptr;
		std::mutex WakeMutex;
		std::condition_variable WakeCondition;
		std::uint64_t WakeEpoch = 0;
		bool bWorkersStopping = false;
		std::vector<TaskWorker*> Workers;
	};

	explicit Implementation(TaskExecutorConfig config);
	~Implementation();

	Implementation(const Implementation&) = delete;
	Implementation& operator=(const Implementation&) = delete;

	std::shared_ptr<TaskExecution::State> Submit(const CompiledTaskGraph& graph, TaskExecutionContext context);
	std::shared_ptr<TaskExecution::State> Launch(const CompiledTaskGraph& graph, TaskExecutionContext context, TaskScope* scope);
	CompiledTaskGraph CompileTask(TaskDesc desc, TaskFunction function) const;
	std::shared_ptr<TaskDispatch::State> PrepareDispatch(TaskScope& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context);
	void EnqueueDispatch(TaskDispatch::State& dispatch) noexcept;
	void ReleaseDispatch(TaskDispatch::State& dispatch) noexcept;
	bool Shutdown(TaskExecutorShutdownMode mode) noexcept;
	std::uint32_t GetWorkerCount(TaskLane lane) const noexcept;

	void Enqueue(ReadyTask task, TaskWorker* preferredWorker, TaskLane lane);
	void OnExecutionSettled();

private:
	enum class LifecycleState : std::uint8_t
	{
		Accepting,
		Draining,
		Cancelling,
		Stopping,
		Stopped
	};

	static void ValidateConfiguration(const TaskExecutorConfig& config);
	static bool RejectExecution(const std::shared_ptr<TaskExecution::State>& execution, std::uint64_t generation, std::string_view reason);
	static std::size_t LaneIndex(TaskLane lane) noexcept;
	static const char* LaneName(TaskLane lane) noexcept;
	std::shared_ptr<TaskExecution::State> CreateExecution(std::uint64_t generation, const std::shared_ptr<TaskScope::State>& scope) const;
	bool ValidateLaunchRequest(
	    const CompiledTaskGraph& graph,
	    const TaskExecutionContext& context,
	    const std::shared_ptr<TaskScope::State>& scope,
	    const std::shared_ptr<TaskExecution::State>& execution,
	    std::uint64_t generation) const;
	bool ValidateWorkerLanes(const CompiledTaskGraph& graph, const std::shared_ptr<TaskExecution::State>& execution, std::uint64_t generation) const;
	bool AdmitExecution(const std::shared_ptr<TaskScope::State>& scope, const std::shared_ptr<TaskExecution::State>& execution, std::uint64_t generation);
	void ReserveExecutionStorage();
	void StartExecution(const CompiledTaskGraph& graph, TaskExecutionContext context, const std::shared_ptr<TaskExecution::State>& execution);
	void ExecuteSerial(const CompiledTaskGraph& graph, TaskExecutionContext& context, const std::shared_ptr<TaskExecution::State>& execution);
	void InitializeWorkers();
	void AddWorkers(TaskLane lane, std::uint32_t count);
	void StartWorkers();
	bool TryPopLocal(TaskWorker& worker, ReadyTask& task);
	bool TryPopInjection(TaskLane lane, ReadyTask& task);
	bool TrySteal(TaskWorker& worker, ReadyTask& task);
	bool TryTakeWork(TaskWorker& worker, ReadyTask& task);
	bool TryPopDispatch(TaskLane lane, ReadyTask& task);
	void ValidateDispatchRequest(const TaskScope::State& scope, const TaskDesc& desc, const TaskFunction& function, const TaskExecutionContext& context) const;
	std::shared_ptr<TaskDispatch::State> CreateDispatch(const std::shared_ptr<TaskScope::State>& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context);
	void AdmitDispatch(const std::shared_ptr<TaskScope::State>& scope, const std::shared_ptr<TaskDispatch::State>& dispatch);
	std::size_t ReserveDispatchSlot();
	void CommitDispatchAdmission(const std::shared_ptr<TaskDispatch::State>& dispatch, std::size_t slot) noexcept;
	void CloseDispatches() noexcept;
	bool WaitForWork(TaskWorker& worker, ReadyTask& task);
	void WorkerMain(TaskWorker& worker);
	std::vector<std::shared_ptr<TaskExecution::State>> BeginShutdown(TaskExecutorShutdownMode mode);
	void RequestCancellation(std::span<const std::shared_ptr<TaskExecution::State>> executions) noexcept;
	void WaitForActiveExecutions();
	void FinishShutdown();
	void RequestWorkerStop() noexcept;
	void JoinWorkers() noexcept;

	static constexpr std::uint32_t MaximumWorkerCount = 256;
	TaskExecutorConfig m_config;
	std::array<TaskLaneState, TaskLaneCount> m_lanes;
	std::vector<std::unique_ptr<TaskWorker>> m_workers;
	std::mutex m_stateMutex;
	std::condition_variable m_stateCondition;
	LifecycleState m_lifecycle = LifecycleState::Accepting;
	std::uint32_t m_activeExecutions = 0;
	std::vector<std::weak_ptr<TaskExecution::State>> m_executions;
	std::vector<std::shared_ptr<TaskDispatch::State>> m_dispatches;
	std::mutex m_shutdownMutex;
	std::atomic_uint64_t m_nextExecutionGeneration{1};
};
