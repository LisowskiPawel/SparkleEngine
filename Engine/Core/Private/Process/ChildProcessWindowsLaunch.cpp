#include "Process/ChildProcessWindows.h"
#include "Process/ChildProcessWindowsArguments.h"

#include <system_error>

#if defined(_WIN32)
namespace Process
{
	static constexpr DWORD ProcessTerminationTimeoutMilliseconds = 5'000;

	static Process::ChildProcessStartFailure ClassifyStartFailure(DWORD code) noexcept
	{
		switch (code)
		{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				return Process::ChildProcessStartFailure::ExecutableNotFound;
			case ERROR_ACCESS_DENIED:
				return Process::ChildProcessStartFailure::AccessDenied;
			case ERROR_ACCESS_DISABLED_BY_POLICY:
			case ERROR_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY:
				return Process::ChildProcessStartFailure::BlockedByPolicy;
			default:
				return Process::ChildProcessStartFailure::OperatingSystemError;
		}
	}

	void TerminateOwnedWindowsProcess(const WindowsOwnedProcess& process) noexcept
	{
		TerminateJobObject(process.Job.Get(), 1);
		WaitForSingleObject(process.Process.Get(), ProcessTerminationTimeoutMilliseconds);
	}

	bool PrepareChildProcessLogDirectory(const std::filesystem::path& logPath, ChildProcessResult& result)
	{
		std::error_code error;
		if (!logPath.parent_path().empty())
		{
			std::filesystem::create_directories(logPath.parent_path(), error);
		}
		if (error)
		{
			result.FailureReason = "Failed to create the child process log directory: " + error.message();
			return false;
		}
		return true;
	}

	static bool CreateSuspendedWindowsChild(
	    const Process::ChildProcessRequest& request,
	    const std::vector<Process::EnvironmentOverride>& environmentOverrides,
	    HANDLE outputHandle,
	    Process::ChildProcessResult& result,
	    WindowsOwnedProcess& outProcess,
	    WindowsProcessHandle& processThread)
	{
		STARTUPINFOW startup{};
		startup.cb = sizeof(startup);
		startup.dwFlags = STARTF_USESTDHANDLES;
		startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startup.hStdOutput = outputHandle;
		startup.hStdError = outputHandle;
		PROCESS_INFORMATION information{};
		std::wstring commandLine = BuildWindowsChildCommandLine(request);
		std::wstring workingDirectory = request.WorkingDirectory.wstring();
		std::vector<wchar_t> environment = BuildWindowsChildEnvironment(environmentOverrides);

		const bool bChildProcessCreationFailed = !CreateProcessW(
		    nullptr,
		    commandLine.data(),
		    nullptr,
		    nullptr,
		    TRUE,
		    CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT | CREATE_SUSPENDED,
		    environment.empty() ? nullptr : environment.data(),
		    workingDirectory.empty() ? nullptr : workingDirectory.c_str(),
		    &startup,
		    &information);

		if (bChildProcessCreationFailed)
		{
			const DWORD startError = GetLastError();
			result.StartFailure = ClassifyStartFailure(startError);
			result.FailureReason = "Failed to launch child process: " + FormatWindowsProcessError(startError);
			return false;
		}

		result.bLaunched = true;
		result.ProcessId = information.dwProcessId;
		outProcess.Process.Reset(information.hProcess);
		processThread.Reset(information.hThread);
		return true;
	}

	static bool EstablishWindowsChildOwnership(WindowsOwnedProcess& outProcess, ChildProcessResult& result)
	{
		outProcess.Job.Reset(CreateJobObjectW(nullptr, nullptr));
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobLimits{};
		jobLimits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		const bool bJobConfigured = outProcess.Job && SetInformationJobObject(outProcess.Job.Get(), JobObjectExtendedLimitInformation, &jobLimits, static_cast<DWORD>(sizeof(jobLimits)));
		const bool bProcessAssigned = bJobConfigured && AssignProcessToJobObject(outProcess.Job.Get(), outProcess.Process.Get());

		if (!bProcessAssigned)
		{
			const DWORD ownershipError = GetLastError();
			TerminateProcess(outProcess.Process.Get(), 1);
			WaitForSingleObject(outProcess.Process.Get(), ProcessTerminationTimeoutMilliseconds);
			result.FailureReason = "Failed to establish child process tree ownership: " + FormatWindowsProcessError(ownershipError);
			return false;
		}
		return true;
	}

	static bool ResumeWindowsChild(const WindowsProcessHandle& processThread, const WindowsOwnedProcess& outProcess, ChildProcessResult& result)
	{
		if (ResumeThread(processThread.Get()) == static_cast<DWORD>(-1))
		{
			const DWORD resumeError = GetLastError();
			TerminateOwnedWindowsProcess(outProcess);
			result.FailureReason = "Failed to start child process: " + FormatWindowsProcessError(resumeError);
			return false;
		}
		return true;
	}

	bool LaunchOwnedWindowsProcess(
	    const ChildProcessRequest& request,
	    const std::vector<EnvironmentOverride>& environmentOverrides,
	    HANDLE outputHandle,
	    ChildProcessResult& result,
	    WindowsOwnedProcess& outProcess)
	{
		WindowsProcessHandle processThread;
		if (!CreateSuspendedWindowsChild(request, environmentOverrides, outputHandle, result, outProcess, processThread))
		{
			return false;
		}
		if (!EstablishWindowsChildOwnership(outProcess, result))
		{
			return false;
		}
		return ResumeWindowsChild(processThread, outProcess, result);
	}
}
#endif
