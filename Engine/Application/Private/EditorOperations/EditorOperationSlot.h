#pragma once

#include "EditorOperations/EditorOperationRuntime.h"
#include "Tasks/Public/TaskExecutor.h"
#include "Tasks/Public/TaskGraph.h"
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
	explicit EditorOperationSlot(EditorOperationRuntime& runtime) noexcept :
	    m_runtime(runtime)
	{
	}

	template <typename TOperation> bool Start(TaskName taskName, std::string_view activeMessage, TOperation operation, std::string& errorMessage) noexcept
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
			TaskGraphBuilder graph;
			graph.Add(TaskDesc{std::move(taskName), TaskLane::BlockingIo}, [operation = std::move(operation), result](TaskExecutionContext& context) mutable { return operation(*result, context); });
			m_execution = m_runtime.GetExecutor().Launch(m_runtime.GetScope(), graph.Compile());
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
	EditorOperationRuntime& m_runtime;
	TaskExecution m_execution;
	std::shared_ptr<TResult> m_result;
};
