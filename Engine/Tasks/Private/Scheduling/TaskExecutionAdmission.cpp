#include "TaskExecutorImplementation.h"

#include "TaskWorkerContext.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>

void TaskExecutor::Implementation::ValidateConfiguration(const TaskExecutorConfig& config)
{
	const std::uint64_t totalWorkers = static_cast<std::uint64_t>(config.FrameCriticalWorkerCount) + config.BackgroundWorkerCount + config.BlockingIoWorkerCount;
	const bool bInvalidSerialMixWorkerCount = config.FrameCriticalWorkerCount == 0;
	const bool bWorkerLanesConfigured = config.BackgroundWorkerCount != 0 || config.BlockingIoWorkerCount != 0;
	const bool bInvalidSerialMix = bInvalidSerialMixWorkerCount && bWorkerLanesConfigured;

	const bool bInvalidCapacityPerExecution = config.MaximumTasksPerExecution == 0 || config.MaximumTasksPerExecution > TaskGraphLimits::HardMaximumTasks;
	const bool bEdgeCapacityInvalid = bInvalidCapacityPerExecution || (config.MaximumEdgesPerExecution > TaskGraphLimits::HardMaximumEdges);
	const bool bInvalidCapacity = bEdgeCapacityInvalid || (config.MaximumActiveExecutions == 0);

	if (totalWorkers > MaximumWorkerCount || bInvalidSerialMix || bInvalidCapacity)
	{
		throw std::invalid_argument("TaskExecutorConfig contains an unsupported lane worker count or capacity.");
	}
}

bool TaskExecutor::Implementation::RejectExecution(const std::shared_ptr<TaskExecution::State>& execution, std::uint64_t generation, std::string_view reason)
{
	TaskExecutionCompletion completion;
	completion.Generation = generation;
	completion.Status = TaskExecutionStatus::Rejected;
	completion.Result = TaskResult::Failure(reason);
	execution->Publish(std::move(completion));
	return false;
}

std::shared_ptr<TaskExecution::State> TaskExecutor::Implementation::CreateExecution(std::uint64_t generation, const std::shared_ptr<TaskScope::State>& scope) const
{
	auto execution = std::make_shared<TaskExecution::State>(generation);
	execution->JoinThread = scope ? scope->OwnerThread : std::this_thread::get_id();
	execution->ExecutorIdentity = this;
	return execution;
}

bool TaskExecutor::Implementation::ValidateWorkerLanes(const CompiledTaskGraph& graph, const std::shared_ptr<TaskExecution::State>& execution, std::uint64_t generation) const
{
	if (m_workers.empty())
	{
		return true;
	}

	for (const TaskGraphNode& node : graph.m_data->Nodes)
	{
		if (GetWorkerCount(node.Desc.Lane) == 0)
		{
			return RejectExecution(execution, generation, "Compiled task graph uses a lane with no configured workers.");
		}
	}

	return true;
}

bool TaskExecutor::Implementation::ValidateLaunchRequest(
    const CompiledTaskGraph& graph,
    const TaskExecutionContext& context,
    const std::shared_ptr<TaskScope::State>& scope,
    const std::shared_ptr<TaskExecution::State>& execution,
    std::uint64_t generation) const
{
	if (TaskWorkerContext::IsWorkerFor(this))
	{
		return RejectExecution(execution, generation, "A task worker cannot submit work to its own executor; use graph dependencies or nested tasks.");
	}
	if (scope && context.HasUserData() && !context.HasOwnedUserData())
	{
		return RejectExecution(execution, generation, "Scoped asynchronous launch received non-owned TaskExecutionContext data.");
	}
	if (!graph.IsValid())
	{
		return RejectExecution(execution, generation, graph.GetError().Message);
	}
	if (graph.GetTaskCount() > m_config.MaximumTasksPerExecution || graph.GetEdgeCount() > m_config.MaximumEdgesPerExecution)
	{
		return RejectExecution(execution, generation, "Compiled task graph exceeds this executor's bounded execution capacity.");
	}

	return ValidateWorkerLanes(graph, execution, generation);
}

void TaskExecutor::Implementation::ReserveExecutionStorage()
{
	std::erase_if(
	    m_executions,
	    [](const auto& item)
	    {
		    const auto execution = item.lock();
		    if (!execution)
		    {
			    return true;
		    }

		    std::lock_guard lock(execution->Mutex);
		    return execution->bSettled;
	    });

	m_executions.reserve(m_executions.size() + 1);
}

bool TaskExecutor::Implementation::AdmitExecution(const std::shared_ptr<TaskScope::State>& scope, const std::shared_ptr<TaskExecution::State>& execution, std::uint64_t generation)
{
	const char* rejection = nullptr;
	{
		std::lock_guard lock(m_stateMutex);
		if (m_lifecycle != LifecycleState::Accepting)
		{
			rejection = "Task executor is no longer accepting submissions.";
		}
		else if (m_activeExecutions >= m_config.MaximumActiveExecutions)
		{
			rejection = "Task executor reached its active-execution capacity.";
		}
		else
		{
			ReserveExecutionStorage();
			const bool bScopeRegistered = !scope || scope->RegisterExecution(execution);
			if (!bScopeRegistered)
			{
				rejection = "TaskScope is closed, settled, or used from a non-owner thread.";
			}
			else
			{
				m_executions.emplace_back(execution);
				++m_activeExecutions;
			}
		}
	}

	// Completion can invoke scope settlement; never publish under the lifecycle lock.
	return rejection == nullptr || RejectExecution(execution, generation, rejection);
}

void TaskExecutor::Implementation::OnExecutionSettled()
{
	{
		std::lock_guard lock(m_stateMutex);
		assert(m_activeExecutions > 0 && "Task executor active execution count underflowed.");
		--m_activeExecutions;
	}
	m_stateCondition.notify_all();
}

CompiledTaskGraph TaskExecutor::Implementation::CompileTask(TaskDesc desc, TaskFunction function) const
{
	TaskGraphBuilder builder(TaskGraphLimits{.MaximumTasks = m_config.MaximumTasksPerExecution, .MaximumEdges = m_config.MaximumEdgesPerExecution});
	builder.Add(std::move(desc), std::move(function));
	return builder.Compile();
}
