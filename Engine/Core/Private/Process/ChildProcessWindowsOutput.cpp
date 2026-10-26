#include "Process/ChildProcessWindows.h"

#include <array>
#include <atomic>
#include <fstream>
#include <stop_token>
#include <string>

#if defined(_WIN32)
namespace Process
{
	static void ConsumeOutput(Process::ChildProcessResult& result, const Process::ChildProcessRequest& request, std::ofstream& log, const char* data, std::size_t size)
	{
		result.CapturedOutput.append(data, size);
		if (request.OutputCallback)
		{
			request.OutputCallback(std::string_view(data, size));
		}
		if (log.is_open())
		{
			log.write(data, static_cast<std::streamsize>(size));
		}
	}

	static bool CreateChildOutputPipe(WindowsProcessHandle& readPipe, WindowsProcessHandle& writePipe, ChildProcessResult& result)
	{
		static std::atomic_uint64_t nextPipeIdentity{1};
		const std::wstring pipeName = L"\\\\.\\pipe\\Sparkle.ChildProcess." + std::to_wstring(GetCurrentProcessId()) + L"." + std::to_wstring(nextPipeIdentity.fetch_add(1, std::memory_order_relaxed));
		readPipe.Reset(CreateNamedPipeW(pipeName.c_str(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 64 * 1024, 64 * 1024, 0, nullptr));
		if (!readPipe)
		{
			result.FailureReason = "Failed to create child output pipe: " + FormatWindowsProcessError(GetLastError());
			return false;
		}

		SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
		writePipe.Reset(CreateFileW(pipeName.c_str(), GENERIC_WRITE, 0, &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
		if (!writePipe)
		{
			result.FailureReason = "Failed to open child output pipe: " + FormatWindowsProcessError(GetLastError());
			return false;
		}
		if (!ConnectNamedPipe(readPipe.Get(), nullptr) && GetLastError() != ERROR_PIPE_CONNECTED)
		{
			result.FailureReason = "Failed to connect child output pipe: " + FormatWindowsProcessError(GetLastError());
			return false;
		}

		return true;
	}

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

	static void CollectChildProcessOutput(const ChildProcessRequest& request, const WindowsOwnedProcess& child, const WindowsProcessHandle& readPipe, std::ofstream& log, ChildProcessResult& result)
	{
		WindowsProcessHandle readEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr));
		WindowsProcessHandle cancelEvent(CreateEventW(nullptr, TRUE, request.Cancellation.stop_requested(), nullptr));
		if (!readEvent || !cancelEvent)
		{
			const DWORD eventError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Failed to create child process completion events: " + FormatWindowsProcessError(eventError);
			return;
		}
		std::stop_callback cancellationWake(request.Cancellation, [event = cancelEvent.Get()] { SetEvent(event); });

		std::array<char, 4096> buffer{};
		OVERLAPPED readOperation{};
		readOperation.hEvent = readEvent.Get();
		bool readPending = false;
		bool pipeClosed = false;
		bool processExited = false;
		bool terminationRequested = false;
		while (!processExited || !pipeClosed)
		{
			if (!pipeClosed && !readPending)
			{
				ResetEvent(readEvent.Get());
				DWORD bytesRead = 0;
				if (ReadFile(readPipe.Get(), buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, &readOperation))
				{
					if (bytesRead == 0)
					{
						pipeClosed = true;
					}
					else
					{
						ConsumeOutput(result, request, log, buffer.data(), bytesRead);
					}
					continue;
				}
				const DWORD readError = GetLastError();
				if (readError == ERROR_IO_PENDING)
				{
					readPending = true;
				}
				else if (readError == ERROR_BROKEN_PIPE)
				{
					pipeClosed = true;
				}
				else
				{
					result.FailureReason = "Failed while reading child output: " + FormatWindowsProcessError(readError);
					pipeClosed = true;
				}
			}

			std::array<HANDLE, 3> waits{};
			DWORD processIndex = MAXDWORD;
			DWORD readIndex = MAXDWORD;
			DWORD cancelIndex = MAXDWORD;
			DWORD count = 0;
			if (!processExited)
			{
				processIndex = count;
				waits[count++] = child.Process.Get();
			}
			if (readPending)
			{
				readIndex = count;
				waits[count++] = readEvent.Get();
			}
			if (!terminationRequested)
			{
				cancelIndex = count;
				waits[count++] = cancelEvent.Get();
			}
			if (count == 0)
			{
				break;
			}
			const DWORD waitResult = WaitForMultipleObjects(count, waits.data(), FALSE, INFINITE);
			if (waitResult == WAIT_FAILED)
			{
				result.FailureReason = "Failed while waiting for child process completion: " + FormatWindowsProcessError(GetLastError());
				TerminateOwnedWindowsProcess(child);
				break;
			}
			const DWORD signalled = waitResult - WAIT_OBJECT_0;
			if (signalled == processIndex)
			{
				processExited = true;
				continue;
			}
			if (signalled == cancelIndex)
			{
				result.Cancelled = true;
				terminationRequested = true;
				TerminateJobObject(child.Job.Get(), 1);
				continue;
			}
			if (signalled == readIndex)
			{
				DWORD bytesRead = 0;
				readPending = false;
				if (GetOverlappedResult(readPipe.Get(), &readOperation, &bytesRead, FALSE) && bytesRead != 0)
				{
					ConsumeOutput(result, request, log, buffer.data(), bytesRead);
				}
				else if (GetLastError() == ERROR_BROKEN_PIPE || bytesRead == 0)
				{
					pipeClosed = true;
				}
				else
				{
					result.FailureReason = "Failed while completing child output read: " + FormatWindowsProcessError(GetLastError());
					pipeClosed = true;
				}
			}
		}

		DWORD exitCode = 1;
		GetExitCodeProcess(child.Process.Get(), &exitCode);
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
