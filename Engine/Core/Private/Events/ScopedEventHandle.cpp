#include "PCH.h"

#include "Core/Public/Events/ScopedEventHandle.h"

#include <utility>

ScopedEventHandle::ScopedEventHandle() noexcept = default;

ScopedEventHandle::~ScopedEventHandle()
{
	Reset();
}

ScopedEventHandle::ScopedEventHandle(ScopedEventHandle&& other) noexcept :
    m_event(std::exchange(other.m_event, nullptr)),
    m_handle(std::exchange(other.m_handle, EventHandle{})),
    m_remove(std::exchange(other.m_remove, nullptr))
{
}

ScopedEventHandle& ScopedEventHandle::operator=(ScopedEventHandle&& other) noexcept
{
	if (this != &other)
	{
		Reset();
		m_event = std::exchange(other.m_event, nullptr);
		m_handle = std::exchange(other.m_handle, EventHandle{});
		m_remove = std::exchange(other.m_remove, nullptr);
	}
	return *this;
}

void ScopedEventHandle::Reset() noexcept
{
	const EventHandle handle = std::exchange(m_handle, EventHandle{});
	void* event = std::exchange(m_event, nullptr);
	const auto remove = std::exchange(m_remove, nullptr);
	if (handle.IsValid())
	{
		remove(event, handle);
	}
}
