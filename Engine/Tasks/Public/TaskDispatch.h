#pragma once

#include "TaskExecution.h"

#include <memory>

class SPARKLE_TASKS_API TaskDispatch final
{
public:
	TaskDispatch() noexcept;
	~TaskDispatch();
	TaskDispatch(TaskDispatch&& other) noexcept;
	TaskDispatch& operator=(TaskDispatch&& other) noexcept;
	TaskDispatch(const TaskDispatch&) = delete;
	TaskDispatch& operator=(const TaskDispatch&) = delete;

	bool Notify() noexcept;
	void Close() noexcept;

	const TaskExecution& GetExecution() const noexcept { return m_execution; }

private:
	friend class TaskExecutor;
	struct State;
	explicit TaskDispatch(std::shared_ptr<State> state) noexcept;

	std::shared_ptr<State> m_state;
	TaskExecution m_execution;
};
