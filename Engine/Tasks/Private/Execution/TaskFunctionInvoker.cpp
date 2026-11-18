#include "TaskFunctionInvoker.h"

#include "TaskExecutionContext.h"

#include <exception>

TaskResult InvokeTaskFunction(const TaskGraphNode& node, TaskExecutionContext& context, bool bBlockedByDependency)
{
	const bool bBlocked = bBlockedByDependency || context.IsCancellationRequested();
	if (bBlocked && node.Desc.CompletionPolicy == TaskCompletionPolicy::Normal)
	{
		return TaskResult::Cancelled("Task execution was cancelled or a prerequisite did not succeed.");
	}

	if (!node.Function)
	{
		return TaskResult::Success();
	}

	try
	{
		return node.Function(context);
	}
	catch (const std::exception& exception)
	{
		return TaskResult::Failure(exception.what());
	}
	catch (...)
	{
		return TaskResult::Failure("Unhandled non-standard task exception.");
	}
}
