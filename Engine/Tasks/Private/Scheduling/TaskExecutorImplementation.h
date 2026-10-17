#pragma once

#include "TaskExecutor.h"

#include <cstdint>
#include <memory>

struct TaskExecutor::Implementation final
{
	explicit Implementation(TaskExecutorConfig config);
	~Implementation();

	Implementation(const Implementation&) = delete;
	Implementation& operator=(const Implementation&) = delete;

	std::shared_ptr<TaskExecution::State> Launch(const CompiledTaskGraph& graph, TaskExecutionContext context, TaskScope* scope);
	std::shared_ptr<TaskDispatch::State> PrepareDispatch(TaskScope& scope, TaskDesc desc, TaskFunction function, TaskExecutionContext context);
	bool Shutdown(TaskExecutorShutdownMode mode) noexcept;
	std::uint32_t GetWorkerCount(TaskLane lane) const noexcept;
	const TaskExecutorConfig& GetConfig() const noexcept;

private:
	friend class TaskDispatch;
	struct Runtime;
	std::unique_ptr<Runtime> m_runtime;
};
