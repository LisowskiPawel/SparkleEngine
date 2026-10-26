#include "Process/ChildProcessWindows.h"

#if defined(_WIN32)
namespace Process
{
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
			result.Cancelled = true;
			return result;
		}
		return request.ReadinessValue.has_value() ? RunWindowsChildProcessUntilReady(request) : RunWindowsChildProcessToExit(request);
	}
}
#endif
