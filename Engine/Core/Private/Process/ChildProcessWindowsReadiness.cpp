#include "Process/ChildProcessWindows.h"
#include "Process/ProcessReadinessEnvironment.h"

#include <array>
#include <atomic>
#include <fstream>
#include <stop_token>
#include <string>
#include <vector>

#if defined(_WIN32)
namespace Process
{
	static void ConsumeLogOutput(Process::ChildProcessResult& result, const Process::ChildProcessRequest& request, std::uint64_t& consumedBytes)
	{
		std::ifstream input(request.LogPath, std::ios::binary);
		if (!input)
		{
			return;
		}
		input.seekg(static_cast<std::streamoff>(consumedBytes));
		std::array<char, 4096> buffer{};
		while (input)
		{
			input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
			const std::streamsize bytesRead = input.gcount();
			if (bytesRead <= 0)
			{
				break;
			}
			result.CapturedOutput.append(buffer.data(), static_cast<std::size_t>(bytesRead));
			if (request.OutputCallback)
			{
				request.OutputCallback(std::string_view(buffer.data(), static_cast<std::size_t>(bytesRead)));
			}
			consumedBytes += static_cast<std::uint64_t>(bytesRead);
		}
	}

	static bool CreateChildReadinessEvent(WindowsProcessHandle& readinessEvent, std::wstring& eventName, ChildProcessResult& result)
	{
		static std::atomic_uint64_t nextReadinessIdentity{1};
		eventName = L"Local\\Sparkle.ProcessReadiness." + std::to_wstring(GetCurrentProcessId()) + L"." + std::to_wstring(nextReadinessIdentity.fetch_add(1, std::memory_order_relaxed));
		readinessEvent.Reset(CreateEventW(nullptr, TRUE, FALSE, eventName.c_str()));
		if (!readinessEvent)
		{
			result.FailureReason = "Failed to create the child readiness event: " + FormatWindowsProcessError(GetLastError());
			return false;
		}
		return true;
	}

	static bool OpenChildReadinessLog(const std::filesystem::path& logPath, WindowsProcessHandle& outputFile, ChildProcessResult& result)
	{
		SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
		outputFile.Reset(CreateFileW(logPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, &security, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
		if (!outputFile)
		{
			result.FailureReason = "Failed to open the child process log: " + FormatWindowsProcessError(GetLastError());
			return false;
		}
		return true;
	}

	static std::vector<EnvironmentOverride> BuildReadinessEnvironment(const ChildProcessRequest& request, std::wstring_view eventName)
	{
		std::vector<Process::EnvironmentOverride> environmentOverrides = request.Environment;
		environmentOverrides.push_back({ReadinessEventEnvironmentVariable, WideToUtf8(eventName)});
		environmentOverrides.push_back({ReadinessValueEnvironmentVariable, *request.ReadinessValue});
		return environmentOverrides;
	}

	static ChildReadinessDecision EvaluateChildReadiness(const ChildProcessRequest& request, ChildProcessResult& result)
	{
		auto decision = Process::ChildReadinessDecision::Ready;
		if (request.ReadinessValidator)
		{
			decision = Process::ChildReadinessDecision::Rejected;
			try
			{
				decision = request.ReadinessValidator(result.ProcessId, result.CapturedOutput, result.FailureReason);
			}
			catch (...)
			{
				result.FailureReason = "Child readiness verification failed.";
			}
		}
		return decision;
	}

	static bool VerifyChildAfterReadiness(const WindowsOwnedProcess& child, ChildProcessResult& result)
	{
		const DWORD childState = WaitForSingleObject(child.Process.Get(), 0);
		if (childState == WAIT_OBJECT_0)
		{
			DWORD exitCode = 1;
			GetExitCodeProcess(child.Process.Get(), &exitCode);
			result.ExitCode = static_cast<int>(exitCode);
			result.FailureReason = "Child exited during readiness verification.";
			return false;
		}
		if (childState != WAIT_TIMEOUT)
		{
			const DWORD waitError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Child process observation failed during readiness verification: " + FormatWindowsProcessError(waitError);
			return false;
		}
		return true;
	}

	static bool ReleaseWindowsProcessOwnership(const WindowsOwnedProcess& child, ChildProcessResult& result)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION releasedLimits{};
		if (!SetInformationJobObject(child.Job.Get(), JobObjectExtendedLimitInformation, &releasedLimits, static_cast<DWORD>(sizeof(releasedLimits))))
		{
			const DWORD releaseError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Application became ready, but process ownership could not be transferred: " + FormatWindowsProcessError(releaseError);
			return false;
		}
		return true;
	}

	static void ValidateAndReleaseChildProcess(const ChildProcessRequest& request, const WindowsOwnedProcess& child, ChildProcessResult& result)
	{
		const auto decision = EvaluateChildReadiness(request, result);
		if (decision == ChildReadinessDecision::Rejected)
		{
			TerminateOwnedWindowsProcess(child);
			return;
		}
		if (decision == ChildReadinessDecision::Ready && !VerifyChildAfterReadiness(child, result))
		{
			return;
		}
		if (!ReleaseWindowsProcessOwnership(child, result))
		{
			return;
		}
		result.Ready = decision == ChildReadinessDecision::Ready;
		result.ExitCode = result.Ready ? 0 : -1;
	}

	static void WaitForChildReadiness(const ChildProcessRequest& request, const WindowsOwnedProcess& child, const WindowsProcessHandle& readinessEvent, ChildProcessResult& result)
	{
		WindowsProcessHandle cancelEvent(CreateEventW(nullptr, TRUE, request.Cancellation.stop_requested(), nullptr));
		if (!cancelEvent)
		{
			const DWORD eventError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Failed to create the child cancellation event: " + FormatWindowsProcessError(eventError);
			return;
		}
		std::stop_callback cancellationWake(request.Cancellation, [event = cancelEvent.Get()] { SetEvent(event); });

		std::uint64_t consumedLogBytes = 0;
		const std::array<HANDLE, 3> waits = {child.Process.Get(), readinessEvent.Get(), cancelEvent.Get()};
		while (true)
		{
			const DWORD waitResult = WaitForMultipleObjects(static_cast<DWORD>(waits.size()), waits.data(), FALSE, 50);
			ConsumeLogOutput(result, request, consumedLogBytes);
			if (waitResult == WAIT_TIMEOUT)
			{
				continue;
			}
			if (waitResult == WAIT_OBJECT_0)
			{
				DWORD exitCode = 1;
				GetExitCodeProcess(child.Process.Get(), &exitCode);
				result.ExitCode = static_cast<int>(exitCode);
				result.FailureReason = "Child process exited before reporting application readiness.";
				return;
			}
			if (waitResult == WAIT_OBJECT_0 + 1)
			{
				ValidateAndReleaseChildProcess(request, child, result);
				return;
			}
			if (waitResult == WAIT_OBJECT_0 + 2)
			{
				result.Cancelled = true;
				TerminateOwnedWindowsProcess(child);
				return;
			}

			const DWORD waitError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Failed while waiting for application readiness: " + FormatWindowsProcessError(waitError);
			return;
		}
	}

	ChildProcessResult RunWindowsChildProcessUntilReady(const ChildProcessRequest& request)
	{
		ChildProcessResult result;
		if (request.LogPath.empty())
		{
			result.FailureReason = "A readiness-based child process requires a durable log path.";
			return result;
		}

		if (!PrepareChildProcessLogDirectory(request.LogPath, result))
		{
			return result;
		}
		WindowsProcessHandle readinessEvent;
		std::wstring eventName;
		if (!CreateChildReadinessEvent(readinessEvent, eventName, result))
		{
			return result;
		}
		WindowsProcessHandle outputFile;
		if (!OpenChildReadinessLog(request.LogPath, outputFile, result))
		{
			return result;
		}
		const auto environmentOverrides = BuildReadinessEnvironment(request, eventName);
		WindowsOwnedProcess child;
		if (!LaunchOwnedWindowsProcess(request, environmentOverrides, outputFile.Get(), result, child))
		{
			return result;
		}
		outputFile.Reset();
		WaitForChildReadiness(request, child, readinessEvent, result);
		return result;
	}
}
#endif
