#include "Process/Windows/ChildProcessWindows.h"

#if defined(_WIN32)
namespace Process
{
	std::string FormatWindowsProcessError(DWORD code)
	{
		return boost::system::system_category().message(static_cast<int>(code));
	}

	ChildProcessResult RunWindowsChildProcess(const ChildProcessRequest& request)
	{
		ChildProcessResult result;
		if (request.ExecutablePath.empty())
		{
			result.FailureReason = "Child process executable path is empty.";
			return result;
		}
		if (request.Cancellation.stop_requested())
		{
			result.bCancelled = true;
			return result;
		}
		return request.ReadinessValue.has_value() ? RunWindowsChildProcessUntilReady(request) : RunWindowsChildProcessToExit(request);
	}
}
#endif
