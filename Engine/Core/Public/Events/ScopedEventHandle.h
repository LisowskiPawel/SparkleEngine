#pragma once

#include "Core/Public/CoreAPI.h"
#include "EventHandle.h"

#include <cstddef>

template <typename Signature, std::size_t Capacity> class Event;

// The event must outlive its subscription. Reset or destroy the subscription before destroying the event.
class ScopedEventHandle final
{
public:
	SPARKLE_CORE_API ScopedEventHandle() noexcept;

	template <typename Signature, std::size_t Capacity> ScopedEventHandle(Event<Signature, Capacity>& event, EventHandle handle) noexcept :
	    m_event(&event),
	    m_handle(handle),
	    m_remove(RemoveSubscription<Signature, Capacity>)
	{
	}

	SPARKLE_CORE_API ~ScopedEventHandle();
	SPARKLE_CORE_API ScopedEventHandle(ScopedEventHandle&& other) noexcept;
	SPARKLE_CORE_API ScopedEventHandle& operator=(ScopedEventHandle&& other) noexcept;

	ScopedEventHandle(const ScopedEventHandle&) = delete;
	ScopedEventHandle& operator=(const ScopedEventHandle&) = delete;

	SPARKLE_CORE_API void Reset() noexcept;

	bool IsValid() const noexcept { return m_handle.IsValid(); }

	EventHandle GetHandle() const noexcept { return m_handle; }

private:
	template <typename Signature, std::size_t Capacity> static void RemoveSubscription(void* event, EventHandle handle) noexcept { static_cast<Event<Signature, Capacity>*>(event)->Remove(handle); }

	void* m_event = nullptr;
	EventHandle m_handle;
	void (*m_remove)(void*, EventHandle) noexcept = nullptr;
};
