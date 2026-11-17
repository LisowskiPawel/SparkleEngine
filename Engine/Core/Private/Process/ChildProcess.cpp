#include "PCH.h"

#include "Process/ChildProcess.h"

#if defined(_WIN32)
  #include "Process/Windows/ChildProcessWindows.h"
#endif

bool Process::ChildProcessResult::Succeeded() const noexcept
{
	const bool bCompleted = bReady || ExitCode == 0;
	const bool bAccepted = bLaunched && !bCancelled;
	return bAccepted && bCompleted && FailureReason.empty();
}

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
