#include "PCH.h"

#include "Process/ChildProcess.h"

#if defined(_WIN32)
  #include "Process/ChildProcessWindows.h"
#endif

Process::ChildProcessResult Process::RunChildProcess(const ChildProcessRequest& request)
{
#if defined(_WIN32)
	return RunWindowsChildProcess(request);
#else
	ChildProcessResult result;
	result.FailureReason = "Child process execution is not implemented for this platform.";
	return result;
#endif
}
