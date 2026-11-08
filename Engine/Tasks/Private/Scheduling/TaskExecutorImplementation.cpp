#include "TaskExecutorImplementation.h"

#include <utility>

TaskExecutor::Implementation::Implementation(TaskExecutorConfig config) :
    m_config(config)
{
	ValidateConfiguration(config);
	InitializeWorkers();
	StartWorkers();
}

TaskExecutor::Implementation::~Implementation()
{
	Shutdown(TaskExecutorShutdownMode::Drain);
}

std::shared_ptr<TaskExecution::State> TaskExecutor::Implementation::Submit(const CompiledTaskGraph& graph, TaskExecutionContext context)
{
	auto execution = Launch(graph, std::move(context), nullptr);
	execution->WaitUntilSettled();
	return execution;
}

std::shared_ptr<TaskExecution::State> TaskExecutor::Implementation::Launch(const CompiledTaskGraph& graph, TaskExecutionContext context, TaskScope* scope)
{
	const std::shared_ptr<TaskScope::State> scopeState = scope != nullptr ? scope->m_state : nullptr;
	const std::uint64_t generation = m_nextExecutionGeneration.fetch_add(1, std::memory_order_relaxed);
	auto execution = CreateExecution(generation, scopeState);

	if (!ValidateLaunchRequest(graph, context, scopeState, execution, generation))
	{
		return execution;
	}
	const bool bAdmitted = AdmitExecution(scopeState, execution, generation);
	if (!bAdmitted)
	{
		return execution;
	}

	StartExecution(graph, std::move(context), execution);
	return execution;
}
