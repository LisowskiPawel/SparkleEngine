#pragma once

#include "EventHandle.h"

#include <cstddef>
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <utility>

template <typename Signature, std::size_t Capacity = 8> class Event;

template <typename... Args, std::size_t Capacity> class Event<void(Args...), Capacity>
{
public:
	using CallbackType = std::function<void(Args...)>;

	Event() noexcept = default;
	~Event() = default;

	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;
	Event(Event&&) = delete;
	Event& operator=(Event&&) = delete;

	EventHandle Add(CallbackType callback) noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (!m_entries[i].Handle.IsValid())
			{
				m_entries[i].Handle.Id = ++m_nextId;
				m_entries[i].Callback = std::move(callback);
				return m_entries[i].Handle;
			}
		}

		assert(false && "Event capacity exceeded. Increase template Capacity parameter.");
		return EventHandle{};
	}

	void Remove(EventHandle handle) noexcept
	{
		if (!handle.IsValid())
		{
			return;
		}

		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_entries[i].Handle == handle)
			{
				m_entries[i].Handle.Invalidate();
				m_entries[i].Callback = nullptr;
				return;
			}
		}
	}

	void Clear() noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			m_entries[i].Handle.Invalidate();
			m_entries[i].Callback = nullptr;
		}
	}

	void Broadcast(Args... args) const noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_entries[i].Handle.IsValid() && m_entries[i].Callback)
			{
				m_entries[i].Callback(args...);
			}
		}
	}

	bool IsBound() const noexcept
	{
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_entries[i].Handle.IsValid())
			{
				return true;
			}
		}
		return false;
	}

	std::size_t GetBoundCount() const noexcept
	{
		std::size_t count = 0;
		for (std::size_t i = 0; i < Capacity; ++i)
		{
			if (m_entries[i].Handle.IsValid())
			{
				++count;
			}
		}
		return count;
	}

	static constexpr std::size_t GetCapacity() noexcept { return Capacity; }

private:
	struct Entry
	{
		EventHandle Handle;
		CallbackType Callback;
	};

	std::array<Entry, Capacity> m_entries{};
	std::uint32_t m_nextId = 0;
};
