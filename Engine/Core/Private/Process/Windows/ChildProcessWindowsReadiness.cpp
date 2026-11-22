#include "Process/Windows/ChildProcessWindows.h"
#include "Process/ProcessReadinessEnvironment.h"
#include "Strings/StringUtils.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <exception>
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
		input.seekg(0, std::ios::end);
		const std::streamoff observedEnd = input.tellg();
		input.seekg(static_cast<std::streamoff>(consumedBytes));
		std::array<char, 4096> buffer{};
		// A growing writer must not keep this poll from reaching readiness or cancellation.
		while (input && static_cast<std::streamoff>(consumedBytes) < observedEnd)
		{
			const auto pendingBytes = observedEnd - static_cast<std::streamoff>(consumedBytes);
			const auto readSize = std::min(static_cast<std::streamoff>(buffer.size()), pendingBytes);
			input.read(buffer.data(), readSize);
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
		const DWORD processId = GetCurrentProcessId();
		const auto readinessIdentity = nextReadinessIdentity.fetch_add(1, std::memory_order_relaxed);
		eventName = L"Local\\Sparkle.ProcessReadiness." + std::to_wstring(processId) + L"." + std::to_wstring(readinessIdentity);
		const HANDLE eventHandle = CreateEventW(nullptr, TRUE, FALSE, eventName.c_str());
		readinessEvent.reset(eventHandle);
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
		const HANDLE logHandle = CreateFileW(logPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, &security, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		outputFile.reset(logHandle);
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
		environmentOverrides.push_back({ReadinessEventEnvironmentVariable, Strings::ToNarrow(eventName)});
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

	static bool VerifyChildAfterReadiness(WindowsOwnedProcess& child, ChildProcessResult& result)
	{
		const DWORD childState = WaitForSingleObject(child.Process.native_handle(), 0);
		if (childState == WAIT_OBJECT_0)
		{
			DWORD exitCode = 1;
			GetExitCodeProcess(child.Process.native_handle(), &exitCode);
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

	static bool ReleaseWindowsProcessOwnership(WindowsOwnedProcess& child, ChildProcessResult& result)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION releasedLimits{};
		const bool bLimitsReleased = SetInformationJobObject(child.Job.get(), JobObjectExtendedLimitInformation, &releasedLimits, static_cast<DWORD>(sizeof(releasedLimits))) != FALSE;

		if (!bLimitsReleased)
		{
			const DWORD releaseError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Application became ready, but process ownership could not be transferred: " + FormatWindowsProcessError(releaseError);
			return false;
		}
		JOBOBJECT_ASSOCIATE_COMPLETION_PORT releasedCompletion{};
		const bool bCompletionReleased = SetInformationJobObject(child.Job.get(), JobObjectAssociateCompletionPortInformation, &releasedCompletion, sizeof(releasedCompletion)) != FALSE;
		if (!bCompletionReleased)
		{
			const DWORD releaseError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Application ownership transfer could not release completion observation: " + FormatWindowsProcessError(releaseError);
			return false;
		}
		return true;
	}

	static void ValidateAndReleaseChildProcess(const ChildProcessRequest& request, WindowsOwnedProcess& child, ChildProcessResult& result)
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
		child.Process.detach();
		child.Job.reset();
		child.JobCompletion.reset();
		result.bReady = decision == ChildReadinessDecision::Ready;
		result.ExitCode = result.bReady ? 0 : -1;
	}

	static void WaitForChildReadiness(const ChildProcessRequest& request, WindowsOwnedProcess& child, const WindowsProcessHandle& readinessEvent, ChildProcessResult& result)
	{
		const bool bCancellationRequested = request.Cancellation.stop_requested();
		const HANDLE cancelEventHandle = CreateEventW(nullptr, TRUE, bCancellationRequested, nullptr);
		WindowsProcessHandle cancelEvent(cancelEventHandle);
		if (!cancelEvent)
		{
			const DWORD eventError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Failed to create the child cancellation event: " + FormatWindowsProcessError(eventError);
			return;
		}

		std::stop_callback cancellationWake(request.Cancellation, [event = cancelEvent.get()] { SetEvent(event); });

		std::uint64_t consumedLogBytes = 0;
		const std::array<HANDLE, 3> waits = {child.Process.native_handle(), readinessEvent.get(), cancelEvent.get()};
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
				GetExitCodeProcess(child.Process.native_handle(), &exitCode);
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
				result.bCancelled = true;
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
		if (!LaunchOwnedWindowsProcess(request, environmentOverrides, outputFile.get(), result, child))
		{
			return result;
		}
		outputFile.reset();
		try
		{
			WaitForChildReadiness(request, child, readinessEvent, result);
		}
		catch (const std::exception& error)
		{
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = error.what();
		}
		catch (...)
		{
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Child process readiness output failed.";
		}
		if (child.Job)
		{
			TerminateOwnedWindowsProcess(child);
			AppendWindowsProcessTerminationFailure(child, result);
		}
		return result;
	}
}
#endif
