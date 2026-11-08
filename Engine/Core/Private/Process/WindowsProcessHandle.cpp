#include "Process/ChildProcessWindows.h"

#include <utility>

#if defined(_WIN32)
namespace Process
{
	WindowsProcessHandle::WindowsProcessHandle(HANDLE handle) noexcept :
	    m_handle(handle)
	{
	}

	WindowsProcessHandle::~WindowsProcessHandle()
	{
		Reset();
	}

	WindowsProcessHandle::WindowsProcessHandle(WindowsProcessHandle&& other) noexcept :
	    m_handle(std::exchange(other.m_handle, nullptr))
	{
	}

	WindowsProcessHandle& WindowsProcessHandle::operator=(WindowsProcessHandle&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}

	WindowsProcessHandle::operator bool() const noexcept
	{
		return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
	}

	void WindowsProcessHandle::Reset(HANDLE handle) noexcept
	{
		if (*this)
		{
			CloseHandle(m_handle);
		}
		m_handle = handle;
	}

}
#endif
