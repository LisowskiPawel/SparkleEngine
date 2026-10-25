#pragma once

#include "TaskTypes.h"

#include <utility>
#include <memory>
#include <stop_token>
#include <typeinfo>

class TaskExecutor;
class TaskDispatch;
class SerialTaskExecution;

class SPARKLE_TASKS_API TaskExecutionContext final
{
public:
	TaskExecutionContext() noexcept;

	template <typename T> explicit TaskExecutionContext(T& value) noexcept :
	    m_userData(&value),
	    m_userType(&typeid(T))
	{
	}

	template <typename T> explicit TaskExecutionContext(std::shared_ptr<T> value) noexcept :
	    m_userData(value.get()),
	    m_userType(&typeid(T)),
	    m_userOwner(std::move(value))
	{
	}

	template <typename T> T* TryGet() noexcept { return m_userType != nullptr && *m_userType == typeid(T) ? static_cast<T*>(m_userData) : nullptr; }

	template <typename T> const T* TryGet() const noexcept { return m_userType != nullptr && *m_userType == typeid(T) ? static_cast<const T*>(m_userData) : nullptr; }

	TaskLane GetLane() const noexcept { return m_lane; }

	bool IsCancellationRequested() const noexcept { return m_cancellation.stop_requested(); }

	std::stop_token GetCancellationToken() const noexcept { return m_cancellation; }

	bool HasUserData() const noexcept { return m_userData != nullptr; }

	bool HasOwnedUserData() const noexcept { return m_userOwner != nullptr; }

private:
	friend class TaskExecutor;
	friend class TaskDispatch;
	friend class SerialTaskExecution;

	void Bind(TaskLane lane, std::stop_token cancellation) noexcept;

	void* m_userData = nullptr;
	const std::type_info* m_userType = nullptr;
	std::shared_ptr<void> m_userOwner;
	std::stop_token m_cancellation;
	TaskLane m_lane = TaskLane::FrameCritical;
};
