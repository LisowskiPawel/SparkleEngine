#include "ScheduledTaskExecution.h"

#include "Execution/TaskFunctionInvoker.h"
#include "Execution/SerialTaskExecution.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Profiling/TaskProfiler.h"

#include <cassert>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogTaskExecutor, "Tasks.Executor");

TaskExecutor::Implementation::ScheduledTaskExecution::ScheduledTaskExecution(
    Implementation& owner,
    std::shared_ptr<const TaskGraphStorage> graph,
    TaskExecutionContext context,
    std::shared_ptr<TaskExecution::State> execution) :
    m_owner(owner),
    m_graph(std::move(graph)),
    m_context(std::move(context)),
    m_execution(std::move(execution)),
    m_generation(m_execution->Data.Generation),
    m_tasks(std::make_unique<ScheduledTaskState[]>(m_graph->Nodes.size())),
    m_taskResults(m_graph->Nodes.size()),
    m_settled(m_graph->Nodes.size(), false)
{
	InitializeTaskStates();
}

void TaskExecutor::Implementation::ScheduledTaskExecution::InitializeTaskStates()
{
	for (std::uint32_t index = 0; index < m_graph->Nodes.size(); ++index)
	{
		const TaskGraphNode& node = m_graph->Nodes[index];
		ScheduledTaskState& task = m_tasks[index];

		task.RemainingPrerequisites.store(static_cast<std::uint32_t>(node.Prerequisites.size()), std::memory_order_relaxed);
		task.UnfinishedCount.store(1u + static_cast<std::uint32_t>(node.NestedChildren.size()), std::memory_order_relaxed);
		task.bParentBodyComplete.store(!node.Parent.has_value(), std::memory_order_relaxed);
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::Start()
{
	RecordDependencies();
	if (m_graph->Nodes.empty())
	{
		Finish();
		return;
	}

	ScheduleInitialTasks();
}

void TaskExecutor::Implementation::ScheduledTaskExecution::RecordDependencies() const
{
	for (std::uint32_t dependent = 0; dependent < m_graph->Nodes.size(); ++dependent)
	{
		for (const std::uint32_t prerequisite : m_graph->Nodes[dependent].Prerequisites)
		{
			TaskProfiler::RecordDependency(m_generation, prerequisite, dependent);
		}
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::ScheduleInitialTasks()
{
	for (std::uint32_t index = 0; index < m_graph->Nodes.size(); ++index)
	{
		TrySchedule(index, nullptr);
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::Execute(std::uint32_t index, TaskWorker& worker)
{
	const TaskGraphNode& node = m_graph->Nodes[index];
	const TaskResult result = ExecuteBody(index, worker);
	RecordTaskResult(index, node, result);
	ReleaseNestedTasks(node, result, worker);
	ReleaseUnfinished(index, &worker);
}

TaskResult TaskExecutor::Implementation::ScheduledTaskExecution::ExecuteBody(std::uint32_t index, const TaskWorker& worker)
{
	const ScheduledTaskState& task = m_tasks[index];
	const TaskGraphNode& node = m_graph->Nodes[index];
	const TaskProfiler::TimePoint taskStart = TaskProfiler::Begin(node.Desc, m_generation, index, worker.LaneWorkerIndex);
	TaskExecutionContext taskContext = m_context;
	taskContext.Bind(node.Desc.Lane, m_execution->Cancellation.get_token());

	const bool bBlockedByDependency = task.bBlockedByPrerequisite.load(std::memory_order_acquire) || task.bBlockedByParent.load(std::memory_order_acquire);
	TaskResult result = InvokeTaskFunction(node, taskContext, bBlockedByDependency);
	TaskProfiler::End(node.Desc, m_generation, index, worker.LaneWorkerIndex, result, taskStart);
	return result;
}

void TaskExecutor::Implementation::ScheduledTaskExecution::RecordTaskResult(std::uint32_t index, const TaskGraphNode& node, const TaskResult& result)
{
	std::lock_guard lock(m_resultMutex);
	m_taskResults[index] = result;
	if (result.Failed() && m_firstFailureTaskName.empty())
	{
		m_firstFailureTaskName = std::string(node.Desc.Name.Get());
		m_firstFailure = result;
	}
	if (result.WasCancelled())
	{
		m_bCancellationObserved = true;
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::ReleaseNestedTasks(const TaskGraphNode& node, const TaskResult& result, TaskWorker& worker)
{
	for (const std::uint32_t childIndex : node.NestedChildren)
	{
		ScheduledTaskState& child = m_tasks[childIndex];
		if (!result.Succeeded())
		{
			child.bBlockedByParent.store(true, std::memory_order_release);
		}

		child.bParentBodyComplete.store(true, std::memory_order_release);
		TrySchedule(childIndex, &worker);
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::TrySchedule(std::uint32_t index, TaskWorker* preferredWorker)
{
	ScheduledTaskState& task = m_tasks[index];
	const bool bPrerequisitesPending = task.RemainingPrerequisites.load(std::memory_order_acquire) != 0 || !task.bParentBodyComplete.load(std::memory_order_acquire);

	if (bPrerequisitesPending)
	{
		return;
	}

	bool bExpected = false;
	if (task.bScheduled.compare_exchange_strong(bExpected, true, std::memory_order_acq_rel))
	{
		const TaskLane lane = m_graph->Nodes[index].Desc.Lane;
		m_owner.Enqueue(ReadyTask{.Execution = shared_from_this(), .TaskIndex = index}, preferredWorker != nullptr && preferredWorker->Lane == lane ? preferredWorker : nullptr, lane);
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::ReleaseUnfinished(std::uint32_t index, TaskWorker* worker)
{
	const std::uint32_t previousCount = m_tasks[index].UnfinishedCount.fetch_sub(1, std::memory_order_acq_rel);
	assert(previousCount != 0 && "Task unfinished count was released more than once.");
	if (previousCount == 1)
	{
		SettleLogicalTask(index, worker);
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::SettleLogicalTask(std::uint32_t index, TaskWorker* worker)
{
	const TaskResult result = RecordLogicalCompletion(index);
	ReleaseDependents(index, result, worker);
	CompleteNestedParent(index, result, worker);
	FinishIfAllTasksSettled();
}

TaskResult TaskExecutor::Implementation::ScheduledTaskExecution::RecordLogicalCompletion(std::uint32_t index)
{
	std::lock_guard lock(m_resultMutex);
	m_settled[index] = true;
	return m_taskResults[index];
}

void TaskExecutor::Implementation::ScheduledTaskExecution::ReleaseDependents(std::uint32_t index, const TaskResult& result, TaskWorker* worker)
{
	for (const std::uint32_t dependentIndex : m_graph->Nodes[index].Dependents)
	{
		ScheduledTaskState& dependent = m_tasks[dependentIndex];
		if (!result.Succeeded())
		{
			dependent.bBlockedByPrerequisite.store(true, std::memory_order_release);
		}
		const std::uint32_t previousPrerequisiteCount = dependent.RemainingPrerequisites.fetch_sub(1, std::memory_order_acq_rel);
		assert(previousPrerequisiteCount != 0 && "Task prerequisite was released more than once.");
		if (previousPrerequisiteCount == 1)
		{
			TrySchedule(dependentIndex, worker);
		}
	}
}

void TaskExecutor::Implementation::ScheduledTaskExecution::CompleteNestedParent(std::uint32_t index, const TaskResult& result, TaskWorker* worker)
{
	if (!m_graph->Nodes[index].Parent.has_value())
	{
		return;
	}

	const std::uint32_t parentIndex = *m_graph->Nodes[index].Parent;
	if (!result.Succeeded())
	{
		std::lock_guard lock(m_resultMutex);
		if (m_taskResults[parentIndex].Succeeded())
		{
			m_taskResults[parentIndex] = result;
		}
	}

	ReleaseUnfinished(parentIndex, worker);
}

void TaskExecutor::Implementation::ScheduledTaskExecution::FinishIfAllTasksSettled()
{
	const std::uint32_t previousSettledCount = m_settledTaskCount.fetch_add(1, std::memory_order_acq_rel);
	assert(previousSettledCount < m_graph->Nodes.size() && "Task execution settled a logical task more than once.");
	if (previousSettledCount + 1u == m_graph->Nodes.size())
	{
		Finish();
	}
}

TaskExecutionCompletion TaskExecutor::Implementation::ScheduledTaskExecution::BuildCompletion()
{
	TaskExecutionCompletion completion;
	completion.Generation = m_generation;
	completion.BuilderIdentity = m_graph->BuilderIdentity;
	completion.BuilderGeneration = m_graph->BuilderGeneration;
	completion.SettledTaskCount = static_cast<std::uint32_t>(m_graph->Nodes.size());

	{
		std::lock_guard lock(m_resultMutex);
		completion.TaskResults = std::move(m_taskResults);
		completion.Settled = std::move(m_settled);
		completion.FirstFailureTaskName = std::move(m_firstFailureTaskName);
		if (!completion.FirstFailureTaskName.empty())
		{
			completion.Status = TaskExecutionStatus::Failed;
			completion.Result = std::move(m_firstFailure);
		}
		else if (m_bCancellationObserved)
		{
			completion.Status = TaskExecutionStatus::Cancelled;
			completion.Result = TaskResult::Cancelled("Task execution contained cancellation.");
		}
	}

	if (completion.Status == TaskExecutionStatus::Invalid)
	{
		completion.Status = TaskExecutionStatus::Succeeded;
		completion.Result = TaskResult::Success();
	}

	return completion;
}

void TaskExecutor::Implementation::ScheduledTaskExecution::Finish()
{
	m_execution->Publish(BuildCompletion());
	m_owner.OnExecutionSettled();
}

void TaskExecutor::Implementation::ExecuteSerial(const CompiledTaskGraph& graph, TaskExecutionContext& context, const std::shared_ptr<TaskExecution::State>& execution)
{
	try
	{
		TaskExecutionCompletion completion = SerialTaskExecution::Execute(*graph.m_data, context, execution->Data.Generation, execution->Cancellation.get_token());
		execution->Publish(std::move(completion));
		OnExecutionSettled();
	}
	catch (...)
	{
		Diagnostics::Fatal(LogTaskExecutor, __FILE__, __LINE__, "Serial task execution failed after admission and could not publish completion.");
	}
}

void TaskExecutor::Implementation::StartExecution(const CompiledTaskGraph& graph, TaskExecutionContext context, const std::shared_ptr<TaskExecution::State>& execution)
{
	if (m_workers.empty())
	{
		ExecuteSerial(graph, context, execution);
		return;
	}

	try
	{
		auto scheduledExecution = std::make_shared<ScheduledTaskExecution>(*this, graph.m_data, std::move(context), execution);
		scheduledExecution->Start();
	}
	catch (...)
	{
		Diagnostics::Fatal(LogTaskExecutor, __FILE__, __LINE__, "Threaded task execution failed after admission and could not safely roll back publication.");
	}
}
