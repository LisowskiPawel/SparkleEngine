#include "Process/Windows/ChildProcessWindows.h"

#if defined(_WIN32)
namespace Process
{
	HANDLE WindowsProcessHandleTraits::make_default() noexcept
	{
		return nullptr;
	}

	bool WindowsProcessHandleTraits::is_allocated(HANDLE handle) noexcept
	{
		return handle != nullptr && handle != INVALID_HANDLE_VALUE;
	}

	void WindowsProcessHandleTraits::operator()(HANDLE handle) const noexcept
	{
		CloseHandle(handle);
	}
}
#endif
