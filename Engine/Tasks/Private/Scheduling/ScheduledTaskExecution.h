#pragma once

#include "Graph/TaskGraphStorage.h"
#include "TaskExecutorImplementation.h"

#include <cstdint>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class TaskExecutor::Implementation::ScheduledTaskExecution final : public std::enable_shared_from_this<ScheduledTaskExecution>
{
public:
	ScheduledTaskExecution(Implementation& owner, std::shared_ptr<const TaskGraphStorage> graph, TaskExecutionContext context, std::shared_ptr<TaskExecution::State> execution);

	void Start();
	void Execute(std::uint32_t index, TaskWorker& worker);

private:
	struct ScheduledTaskState final
	{
		std::atomic_uint32_t RemainingPrerequisites{0};
		std::atomic_uint32_t UnfinishedCount{1};
		std::atomic_bool bParentBodyComplete{true};
		std::atomic_bool bBlockedByPrerequisite{false};
		std::atomic_bool bBlockedByParent{false};
		std::atomic_bool bScheduled{false};
	};

	void InitializeTaskStates();
	void RecordDependencies() const;
	void ScheduleInitialTasks();
	TaskResult ExecuteBody(std::uint32_t index, const TaskWorker& worker);
	void RecordTaskResult(std::uint32_t index, const TaskGraphNode& node, const TaskResult& result);
	void ReleaseNestedTasks(const TaskGraphNode& node, const TaskResult& result, TaskWorker& worker);
	void TrySchedule(std::uint32_t index, TaskWorker* preferredWorker);
	void ReleaseUnfinished(std::uint32_t index, TaskWorker* worker);
	void SettleLogicalTask(std::uint32_t index, TaskWorker* worker);
	TaskResult RecordLogicalCompletion(std::uint32_t index);
	void ReleaseDependents(std::uint32_t index, const TaskResult& result, TaskWorker* worker);
	void CompleteNestedParent(std::uint32_t index, const TaskResult& result, TaskWorker* worker);
	void FinishIfAllTasksSettled();
	TaskExecutionCompletion BuildCompletion();
	void Finish();

	Implementation& m_owner;
	std::shared_ptr<const TaskGraphStorage> m_graph;
	TaskExecutionContext m_context;
	std::shared_ptr<TaskExecution::State> m_execution;
	std::uint64_t m_generation = 0;
	std::unique_ptr<ScheduledTaskState[]> m_tasks;
	std::mutex m_resultMutex;
	std::vector<TaskResult> m_taskResults;
	std::vector<bool> m_settled;
	std::string m_firstFailureTaskName;
	TaskResult m_firstFailure = TaskResult::Success();
	// The last logical settler acquires prior settlements before moving results into terminal publication.
	std::atomic_uint32_t m_settledTaskCount{0};
	bool m_bCancellationObserved = false;
};
