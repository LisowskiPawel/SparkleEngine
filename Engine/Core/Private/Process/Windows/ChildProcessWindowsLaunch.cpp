#include "Process/Windows/ChildProcessWindows.h"
#include "Process/Windows/ChildProcessWindowsArguments.h"
#include "Strings/StringUtils.h"

#if defined(_WIN32)
  #include <boost/process/v2/environment.hpp>
  #include <boost/process/v2/start_dir.hpp>
  #include <boost/process/v2/stdio.hpp>
  #include <boost/process/v2/windows/default_launcher.hpp>
  #include <algorithm>
  #include <cstddef>
  #include <system_error>

namespace Process
{
	namespace ChildProcess = boost::process::v2;
	static constexpr DWORD ProcessTerminationTimeoutMilliseconds = 5'000;

	static DWORD TerminateWindowsProcessTree(WindowsOwnedProcess& child, HANDLE root) noexcept
	{
		if (child.TerminationError != ERROR_IO_PENDING)
		{
			return child.TerminationError;
		}
		const ULONGLONG deadline = GetTickCount64() + ProcessTerminationTimeoutMilliseconds;
		if (!TerminateJobObject(child.Job.get(), 1))
		{
			return child.TerminationError = GetLastError();
		}
		const ULONGLONG beforeRootWait = GetTickCount64();
		const DWORD rootBudget = beforeRootWait < deadline ? static_cast<DWORD>(deadline - beforeRootWait) : 0;
		const DWORD rootWait = WaitForSingleObject(root, rootBudget);
		if (rootWait != WAIT_OBJECT_0)
		{
			return child.TerminationError = rootWait == WAIT_FAILED ? GetLastError() : ERROR_TIMEOUT;
		}
		while (true)
		{
			const ULONGLONG now = GetTickCount64();
			if (now >= deadline)
			{
				return child.TerminationError = ERROR_TIMEOUT;
			}
			DWORD message = 0;
			ULONG_PTR key = 0;
			OVERLAPPED* data = nullptr;
			const DWORD remaining = static_cast<DWORD>(deadline - now);
			if (!GetQueuedCompletionStatus(child.JobCompletion.get(), &message, &key, &data, remaining))
			{
				const DWORD error = GetLastError();
				return child.TerminationError = error == WAIT_TIMEOUT ? ERROR_TIMEOUT : error;
			}
			const bool bOwnJob = key == reinterpret_cast<ULONG_PTR>(child.Job.get());
			if (bOwnJob && message == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)
			{
				JOBOBJECT_BASIC_ACCOUNTING_INFORMATION accounting{};
				const bool bAccountingRead = QueryInformationJobObject(child.Job.get(), JobObjectBasicAccountingInformation, &accounting, sizeof(accounting), nullptr) != FALSE;
				if (!bAccountingRead)
				{
					return child.TerminationError = GetLastError();
				}
				if (accounting.ActiveProcesses == 0)
				{
					return child.TerminationError = ERROR_SUCCESS;
				}
			}
		}
	}

	static boost::system::error_code LastProcessError() noexcept
	{
		const DWORD error = GetLastError();
		return {static_cast<int>(error), boost::system::system_category()};
	}

	struct WindowsChildLaunch final
	{
		WindowsOwnedProcess& Owner;
		ChildProcessResult& Result;
		HANDLE Job = Owner.Job.get();
		WindowsProcessHandle RawProcess;
		std::vector<std::byte> AttributeStorage;
		LPPROC_THREAD_ATTRIBUTE_LIST Attributes = nullptr;

		~WindowsChildLaunch()
		{
			if (RawProcess)
			{
				TerminateWindowsProcessTree(Owner, RawProcess.get());
			}
			if (Attributes)
			{
				DeleteProcThreadAttributeList(Attributes);
			}
		}

		boost::system::error_code on_setup(ChildProcess::windows::default_launcher& launcher, const std::filesystem::path&, std::wstring&)
		{
			SIZE_T size = 0;
			InitializeProcThreadAttributeList(nullptr, 2, 0, &size);
			if (size == 0)
			{
				return LastProcessError();
			}
			AttributeStorage.resize(size);
			auto* attributes = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(AttributeStorage.data());
			if (!InitializeProcThreadAttributeList(attributes, 2, 0, &size))
			{
				return LastProcessError();
			}
			Attributes = attributes;
			const bool bJobAdded = UpdateProcThreadAttribute(Attributes, 0, PROC_THREAD_ATTRIBUTE_JOB_LIST, &Job, sizeof(Job), nullptr, nullptr) != FALSE;
			if (!bJobAdded)
			{
				return LastProcessError();
			}
			launcher.startup_info.lpAttributeList = Attributes;
			launcher.creation_flags |= CREATE_NO_WINDOW;
			std::erase_if(launcher.inherited_handles, [](HANDLE handle) { return handle == nullptr || handle == INVALID_HANDLE_VALUE; });
			return {};
		}

		void on_success(ChildProcess::windows::default_launcher& launcher, const std::filesystem::path&, std::wstring&) noexcept
		{
			// Guard native creation until the launcher's managed handle construction returns.
			RawProcess.reset(launcher.process_information.hProcess);
			Result.bLaunched = true;
			Result.ProcessId = launcher.process_information.dwProcessId;
		}
	};

	static ChildProcess::process_environment BuildChildEnvironment(const std::vector<EnvironmentOverride>& overrides)
	{
		std::vector<ChildProcess::environment::key_value_pair> values;
		for (auto entry : ChildProcess::environment::current())
		{
			values.emplace_back(entry);
		}
		for (const EnvironmentOverride& overrideValue : overrides)
		{
			const auto name = Strings::ToWide(overrideValue.Name);
			const auto value = Strings::ToWide(overrideValue.Value);
			const ChildProcess::environment::key_view key(name.c_str());
			std::erase_if(values, [&key](const auto& entry) { return entry.key() == key; });
			values.emplace_back(key, ChildProcess::environment::value_view(value));
		}
		return ChildProcess::process_environment(values);
	}

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

	void TerminateOwnedWindowsProcess(WindowsOwnedProcess& process) noexcept
	{
		TerminateWindowsProcessTree(process, process.Process.native_handle());
	}

	void AppendWindowsProcessTerminationFailure(const WindowsOwnedProcess& process, ChildProcessResult& result)
	{
		if (process.TerminationError != ERROR_SUCCESS && process.TerminationError != ERROR_IO_PENDING)
		{
			if (!result.FailureReason.empty())
			{
				result.FailureReason += " ";
			}
			result.FailureReason += "Failed to settle the child process tree: " + FormatWindowsProcessError(process.TerminationError);
		}
	}

	bool LaunchOwnedWindowsProcess(
	    const ChildProcessRequest& request,
	    const std::vector<EnvironmentOverride>& environmentOverrides,
	    HANDLE outputHandle,
	    ChildProcessResult& result,
	    WindowsOwnedProcess& child)
	{
		child.Job.reset(CreateJobObjectW(nullptr, nullptr));
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
		limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		const bool bJobConfigured = child.Job && SetInformationJobObject(child.Job.get(), JobObjectExtendedLimitInformation, &limits, sizeof(limits));
		if (!bJobConfigured)
		{
			result.FailureReason = "Failed to establish child process tree ownership: " + FormatWindowsProcessError(GetLastError());
			return false;
		}
		child.JobCompletion.reset(CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1));
		JOBOBJECT_ASSOCIATE_COMPLETION_PORT completion{child.Job.get(), child.JobCompletion.get()};
		const bool bCompletionAssociated = child.JobCompletion && SetInformationJobObject(child.Job.get(), JobObjectAssociateCompletionPortInformation, &completion, sizeof(completion));
		if (!bCompletionAssociated)
		{
			result.FailureReason = "Failed to observe child process tree completion: " + FormatWindowsProcessError(GetLastError());
			return false;
		}
		try
		{
			WindowsProcessHandle input;
			const HANDLE parentInput = GetStdHandle(STD_INPUT_HANDLE);
			if (parentInput && parentInput != INVALID_HANDLE_VALUE)
			{
				HANDLE inheritedInput = nullptr;
				const bool bInputCopied = DuplicateHandle(GetCurrentProcess(), parentInput, GetCurrentProcess(), &inheritedInput, 0, TRUE, DUPLICATE_SAME_ACCESS) != FALSE;
				if (!bInputCopied)
				{
					result.FailureReason = "Failed to inherit child input: " + FormatWindowsProcessError(GetLastError());
					return false;
				}
				input.reset(inheritedInput);
			}
			auto commandLine = BuildWindowsChildCommandLine(request);
			auto environment = BuildChildEnvironment(environmentOverrides);
			ChildProcess::process_stdio stdio{{}, outputHandle, outputHandle};
			if (input)
			{
				stdio.in = ChildProcess::process_stdio{input.get(), outputHandle, outputHandle}.in;
			}
			WindowsChildLaunch launch{child, result};
			ChildProcess::windows::default_launcher launcher;
			boost::system::error_code error;
			// An empty application path preserves CreateProcess command-line executable lookup.
			auto process = launcher(
			    child.Context.get_executor(),
			    error,
			    std::filesystem::path{},
			    commandLine.c_str(),
			    stdio,
			    environment,
			    ChildProcess::process_start_dir(request.WorkingDirectory),
			    launch);
			launch.RawProcess.release();
			if (error)
			{
				result.StartFailure = ClassifyStartFailure(error.value());
				result.FailureReason = "Failed to launch child process: " + error.message();
				return false;
			}
			child.Process = std::move(process);
			return true;
		}
		catch (const std::exception& error)
		{
			result.FailureReason = "Failed to prepare child process: " + std::string(error.what());
			AppendWindowsProcessTerminationFailure(child, result);
			return false;
		}
	}
}
#endif
