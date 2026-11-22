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
  #include <boost/scope/unique_resource.hpp>

namespace Process
{
	struct WindowsProcessHandleTraits final
	{
		static HANDLE make_default() noexcept;
		static bool is_allocated(HANDLE handle) noexcept;
		void operator()(HANDLE handle) const noexcept;
	};

	using WindowsProcessHandle = boost::scope::unique_resource<HANDLE, WindowsProcessHandleTraits, WindowsProcessHandleTraits>;

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
