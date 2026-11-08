#include "TaskExecutor.h"

#include "TaskExecutorImplementation.h"

#include <utility>

TaskExecutor::TaskExecutor(TaskExecutorConfig config) :
    m_implementation(std::make_unique<Implementation>(config))
{
}

TaskExecutor::~TaskExecutor() = default;

TaskExecution TaskExecutor::Submit(const CompiledTaskGraph& graph, TaskExecutionContext& context)
{
	return TaskExecution(m_implementation->Submit(graph, context));
}

TaskExecution TaskExecutor::Submit(TaskDesc desc, TaskFunction function, TaskExecutionContext& context)
{
	const CompiledTaskGraph graph = m_implementation->CompileTask(std::move(desc), std::move(function));
	return Submit(graph, context);
}

TaskExecution TaskExecutor::Launch(TaskScope& scope, const CompiledTaskGraph& graph, TaskExecutionContext context)
{
	return TaskExecution(m_implementation->Launch(graph, std::move(context), &scope));
}

TaskExecution TaskExecutor::Launch(TaskScope& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context)
{
	const CompiledTaskGraph graph = m_implementation->CompileTask(std::move(desc), std::move(function));
	return Launch(scope, graph, std::move(context));
}

TaskDispatch TaskExecutor::PrepareDispatch(TaskScope& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context)
{
	return TaskDispatch(m_implementation->PrepareDispatch(scope, std::move(desc), std::move(function), std::move(context)));
}

bool TaskExecutor::Shutdown(TaskExecutorShutdownMode mode) noexcept
{
	return m_implementation->Shutdown(mode);
}

std::uint32_t TaskExecutor::GetWorkerCount(TaskLane lane) const noexcept
{
	return m_implementation->GetWorkerCount(lane);
}
