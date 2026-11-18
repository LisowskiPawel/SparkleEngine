#pragma once

#include "TasksAPI.h"

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

class TaskExecutionContext;

enum class TaskCompletionPolicy : std::uint8_t
{
	Normal,
	Cleanup
};

enum class TaskLane : std::uint8_t
{
	FrameCritical,
	Background,
	BlockingIo
};

enum class TaskOutcome : std::uint8_t
{
	Succeeded,
	Failed,
	Cancelled
};

class SPARKLE_TASKS_API TaskResult final
{
public:
	TaskResult() noexcept;

	static TaskResult Success() noexcept;
	static TaskResult Failure(std::string_view message);
	static TaskResult Cancelled(std::string_view reason = {});

	TaskOutcome GetOutcome() const noexcept { return m_outcome; }

	std::string_view GetDiagnostic() const noexcept { return m_message; }

	bool Succeeded() const noexcept { return m_outcome == TaskOutcome::Succeeded; }

	bool Failed() const noexcept { return m_outcome == TaskOutcome::Failed; }

	bool WasCancelled() const noexcept { return m_outcome == TaskOutcome::Cancelled; }

	bool operator==(const TaskResult&) const noexcept;

private:
	TaskResult(TaskOutcome outcome, std::string_view message);

	TaskOutcome m_outcome = TaskOutcome::Succeeded;
	std::string m_message;
};

using TaskFunction = std::function<TaskResult(TaskExecutionContext&)>;

struct TaskDesc final
{
	std::string Name;
	TaskLane Lane = TaskLane::FrameCritical;
	TaskCompletionPolicy CompletionPolicy = TaskCompletionPolicy::Normal;
};
