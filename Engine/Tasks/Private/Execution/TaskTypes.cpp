#include "TaskTypes.h"

TaskResult::TaskResult() noexcept = default;

TaskResult::TaskResult(TaskOutcome outcome, std::string_view message) :
    m_outcome(outcome),
    m_message(message)
{
}

TaskResult TaskResult::Success() noexcept
{
	return TaskResult(TaskOutcome::Succeeded, {});
}

TaskResult TaskResult::Failure(std::string_view message)
{
	return TaskResult(TaskOutcome::Failed, message);
}

TaskResult TaskResult::Cancelled(std::string_view reason)
{
	return TaskResult(TaskOutcome::Cancelled, reason);
}

bool TaskResult::operator==(const TaskResult&) const noexcept = default;
