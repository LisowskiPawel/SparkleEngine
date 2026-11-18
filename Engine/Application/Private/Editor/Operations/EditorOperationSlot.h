#pragma once

#include "Tasks/Public/TaskExecutor.h"
#include "Tasks/Public/TaskTypes.h"

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

// Private scheduling primitive. The owning feature supplies the concrete task and result type.
template <typename TResult> class EditorOperationSlot final
{
public:
	EditorOperationSlot(TaskExecutor& executor, TaskScope& scope) noexcept :
	    m_executor(executor),
	    m_scope(scope)
	{
	}

	template <typename TOperation> bool Start(std::string taskName, std::string_view activeMessage, TOperation operation, std::string& errorMessage)
	{
		if (m_execution.IsValid())
		{
			errorMessage = activeMessage;
			return false;
		}

		try
		{
			m_result = std::make_shared<TResult>();
			const auto result = m_result;
			m_execution = m_executor.Launch(
			    m_scope,
			    TaskDesc{std::move(taskName), TaskLane::BlockingIo},
			    [operation = std::move(operation), result](TaskExecutionContext& context) mutable { return operation(*result, context); });
			errorMessage.clear();
			return true;
		}
		catch (const std::exception& exception)
		{
			m_result.reset();
			errorMessage = exception.what();
		}
		catch (...)
		{
			m_result.reset();
			errorMessage = "Unknown editor operation launch failure.";
		}
		return false;
	}

	bool IsOccupied() const noexcept { return m_execution.IsValid(); }

	std::optional<TaskResult> TryConsume(TResult& result)
	{
		if (!m_execution.IsValid() || !m_execution.IsSettled())
		{
			return std::nullopt;
		}

		TaskResult taskResult = m_execution.GetResult();
		result = std::move(*m_result);
		m_result.reset();
		m_execution = {};
		return taskResult;
	}

private:
	TaskExecutor& m_executor;
	TaskScope& m_scope;
	TaskExecution m_execution;
	std::shared_ptr<TResult> m_result;
};
