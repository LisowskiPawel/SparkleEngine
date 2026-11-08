#include "Process/ChildProcessWindows.h"
#include "Process/ChildProcessWindowsPipe.h"

#include <array>
#include <fstream>
#include <stop_token>
#include <string>

#if defined(_WIN32)
namespace Process
{
	static bool OpenChildOutputLog(const std::filesystem::path& logPath, std::ofstream& log, ChildProcessResult& result)
	{
		if (!logPath.empty())
		{
			if (!PrepareChildProcessLogDirectory(logPath, result))
			{
				return false;
			}

			log.open(logPath, std::ios::binary | std::ios::trunc);
			if (!log)
			{
				result.FailureReason = "Failed to open the child process log.";
				return false;
			}
		}

		return true;
	}

	enum class ChildOutputEvent
	{
		ProcessExited,
		OutputReady,
		Cancelled,
		Finished,
		Failed
	};

	static ChildOutputEvent WaitForChildOutputEvent(
	    const WindowsProcessHandle& process,
	    const WindowsChildOutputReader& reader,
	    const WindowsProcessHandle& cancelEvent,
	    bool bProcessExited,
	    bool bTerminationRequested,
	    ChildProcessResult& result)
	{
		std::array<HANDLE, 3> waits{};
		DWORD processIndex = MAXDWORD;
		DWORD cancelIndex = MAXDWORD;
		DWORD count = 0;
		if (!bProcessExited)
		{
			processIndex = count;
			waits[count++] = process.Get();
		}
		if (reader.HasPendingRead())
		{
			waits[count++] = reader.GetReadEvent();
		}
		if (!bTerminationRequested)
		{
			cancelIndex = count;
			waits[count++] = cancelEvent.Get();
		}
		if (count == 0)
		{
			return ChildOutputEvent::Finished;
		}
		const DWORD waitResult = WaitForMultipleObjects(count, waits.data(), FALSE, INFINITE);
		if (waitResult == WAIT_FAILED)
		{
			result.FailureReason = "Failed while waiting for child process completion: " + FormatWindowsProcessError(GetLastError());
			return ChildOutputEvent::Failed;
		}
		const DWORD signalled = waitResult - WAIT_OBJECT_0;
		if (signalled == processIndex)
		{
			return ChildOutputEvent::ProcessExited;
		}
		if (signalled == cancelIndex)
		{
			return ChildOutputEvent::Cancelled;
		}
		return ChildOutputEvent::OutputReady;
	}

	static void FinishChildProcessOutput(const WindowsProcessHandle& process, std::ofstream& log, ChildProcessResult& result)
	{
		DWORD exitCode = 1;
		GetExitCodeProcess(process.Get(), &exitCode);
		result.ExitCode = static_cast<int>(exitCode);
		if (log.is_open())
		{
			log.flush();
			if (!log && result.FailureReason.empty())
			{
				result.FailureReason = "Failed to write the child process log.";
			}
		}
	}

	static void CollectChildProcessOutput(const ChildProcessRequest& request, const WindowsOwnedProcess& child, const WindowsProcessHandle& readPipe, std::ofstream& log, ChildProcessResult& result)
	{
		WindowsChildOutputReader outputReader(readPipe.Get());
		WindowsProcessHandle cancelEvent(CreateEventW(nullptr, TRUE, request.Cancellation.stop_requested(), nullptr));
		if (!outputReader.IsValid() || !cancelEvent)
		{
			const DWORD eventError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Failed to create child process completion events: " + FormatWindowsProcessError(eventError);
			return;
		}
		std::stop_callback cancellationWake(request.Cancellation, [event = cancelEvent.Get()] { SetEvent(event); });

		bool bProcessExited = false;
		bool bTerminationRequested = false;
		while (!bProcessExited || !outputReader.IsClosed())
		{
			if (outputReader.ReadAvailable(request.OutputCallback, log, result))
			{
				continue;
			}

			const ChildOutputEvent event = WaitForChildOutputEvent(child.Process, outputReader, cancelEvent, bProcessExited, bTerminationRequested, result);
			if (event == ChildOutputEvent::Finished)
			{
				break;
			}
			if (event == ChildOutputEvent::Failed)
			{
				TerminateOwnedWindowsProcess(child);
				break;
			}
			if (event == ChildOutputEvent::ProcessExited)
			{
				bProcessExited = true;
				continue;
			}
			if (event == ChildOutputEvent::Cancelled)
			{
				result.bCancelled = true;
				bTerminationRequested = true;
				TerminateJobObject(child.Job.Get(), 1);
				continue;
			}
			outputReader.CompleteRead(request.OutputCallback, log, result);
		}

		FinishChildProcessOutput(child.Process, log, result);
	}

	ChildProcessResult RunWindowsChildProcessToExit(const ChildProcessRequest& request)
	{
		ChildProcessResult result;
		WindowsProcessHandle readPipe;
		WindowsProcessHandle writePipe;
		if (!CreateChildOutputPipe(readPipe, writePipe, result))
		{
			return result;
		}
		std::ofstream log;
		if (!OpenChildOutputLog(request.LogPath, log, result))
		{
			return result;
		}
		WindowsOwnedProcess child;
		if (!LaunchOwnedWindowsProcess(request, request.Environment, writePipe.Get(), result, child))
		{
			return result;
		}
		writePipe.Reset();
		CollectChildProcessOutput(request, child, readPipe, log, result);
		return result;
	}
}
#endif
