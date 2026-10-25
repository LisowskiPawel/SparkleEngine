#include "TaskExecutionContext.h"

#include <utility>

TaskExecutionContext::TaskExecutionContext() noexcept = default;

void TaskExecutionContext::Bind(TaskLane lane, std::stop_token cancellation) noexcept
{
	m_lane = lane;
	m_cancellation = std::move(cancellation);
}
