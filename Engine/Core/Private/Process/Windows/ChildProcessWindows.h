#pragma once

#include "Process/ChildProcess.h"

#if defined(_WIN32)
  #ifndef NOMINMAX
	#define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
  #endif
  #include <Windows.h>

  #include <string>
  #include <string_view>
  #include <vector>
  #include <boost/asio/io_context.hpp>
  #include <boost/process/v2/process.hpp>

namespace Process
{
	class WindowsProcessHandle final
	{
	public:
		WindowsProcessHandle() noexcept = default;
		explicit WindowsProcessHandle(HANDLE handle) noexcept;
		~WindowsProcessHandle();
		WindowsProcessHandle(const WindowsProcessHandle&) = delete;
		WindowsProcessHandle& operator=(const WindowsProcessHandle&) = delete;
		WindowsProcessHandle(WindowsProcessHandle&& other) noexcept;
		WindowsProcessHandle& operator=(WindowsProcessHandle&& other) noexcept;

		HANDLE Get() const noexcept { return m_handle; }

		explicit operator bool() const noexcept;
		void Reset(HANDLE handle = nullptr) noexcept;
		HANDLE Release() noexcept;

	private:
		HANDLE m_handle = nullptr;
	};

	struct WindowsOwnedProcess final
	{
		boost::asio::io_context Context;
		boost::process::v2::basic_process<boost::asio::io_context::executor_type> Process{Context.get_executor()};
		WindowsProcessHandle Job;
		WindowsProcessHandle JobCompletion;
		DWORD TerminationError = ERROR_IO_PENDING;
	};

	std::string FormatWindowsProcessError(DWORD code);
	void TerminateOwnedWindowsProcess(WindowsOwnedProcess& process) noexcept;
	void AppendWindowsProcessTerminationFailure(const WindowsOwnedProcess& process, ChildProcessResult& result);
	bool PrepareChildProcessLogDirectory(const std::filesystem::path& logPath, ChildProcessResult& result);
	bool LaunchOwnedWindowsProcess(
	    const ChildProcessRequest& request,
	    const std::vector<EnvironmentOverride>& environmentOverrides,
	    HANDLE outputHandle,
	    ChildProcessResult& result,
	    WindowsOwnedProcess& outProcess);
	ChildProcessResult RunWindowsChildProcess(const ChildProcessRequest& request);
	ChildProcessResult RunWindowsChildProcessUntilReady(const ChildProcessRequest& request);
	ChildProcessResult RunWindowsChildProcessToExit(const ChildProcessRequest& request);
}
#endif
