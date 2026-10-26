#include "Process/ChildProcessWindows.h"
#include "Process/CommandLineUtils.h"

#include <algorithm>
#include <cwchar>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#if defined(_WIN32)
namespace Process
{
	static constexpr DWORD ProcessTerminationTimeoutMilliseconds = 5'000;

	WindowsProcessHandle::WindowsProcessHandle(HANDLE handle) noexcept : m_handle(handle) {}
	WindowsProcessHandle::~WindowsProcessHandle() { Reset(); }
	WindowsProcessHandle::WindowsProcessHandle(WindowsProcessHandle&& other) noexcept : m_handle(std::exchange(other.m_handle, nullptr)) {}
	WindowsProcessHandle& WindowsProcessHandle::operator=(WindowsProcessHandle&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}
	WindowsProcessHandle::operator bool() const noexcept { return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE; }
	void WindowsProcessHandle::Reset(HANDLE handle) noexcept
	{
		if (*this) { CloseHandle(m_handle); }
		m_handle = handle;
	}

	static std::wstring Utf8ToWide(std::string_view text)
	{
		if (text.empty())
		{
			return {};
		}
		const int length = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
		if (length <= 0)
		{
			return std::wstring(text.begin(), text.end());
		}
		std::wstring result(static_cast<std::size_t>(length), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), length);
		return result;
	}


	std::string WideToUtf8(std::wstring_view text)
	{
		if (text.empty())
		{
			return {};
		}
		const int length = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
		if (length <= 0)
		{
			return "Unknown Windows error";
		}
		std::string result(static_cast<std::size_t>(length), '\0');
		WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), length, nullptr, nullptr);
		return result;
	}


	static std::wstring BuildCommandLine(const Process::ChildProcessRequest& request)
	{
		std::string commandLine = CommandLine::QuotePath(request.ExecutablePath);
		for (const std::string& argument : request.Arguments)
		{
			commandLine.push_back(' ');
			commandLine += CommandLine::QuoteArgument(argument);
		}
		return Utf8ToWide(commandLine);
	}


	static std::vector<wchar_t> BuildEnvironment(const std::vector<Process::EnvironmentOverride>& overrides)
	{
		if (overrides.empty())
		{
			return {};
		}
		std::vector<std::wstring> entries;
		if (LPWCH environment = GetEnvironmentStringsW())
		{
			for (const wchar_t* current = environment; *current != L'\0'; current += std::wcslen(current) + 1)
			{
				entries.emplace_back(current);
			}
			FreeEnvironmentStringsW(environment);
		}
		for (const Process::EnvironmentOverride& overrideValue : overrides)
		{
			const std::wstring prefix = Utf8ToWide(overrideValue.Name) + L"=";
			const std::wstring replacement = prefix + Utf8ToWide(overrideValue.Value);
			auto entry = std::find_if(entries.begin(), entries.end(), [&prefix](const std::wstring& value) { return value.rfind(prefix, 0) == 0; });
			if (entry == entries.end())
			{
				entries.push_back(replacement);
			}
			else
			{
				*entry = replacement;
			}
		}
		std::sort(entries.begin(), entries.end());
		std::vector<wchar_t> block;
		for (const std::wstring& entry : entries)
		{
			block.insert(block.end(), entry.begin(), entry.end());
			block.push_back(L'\0');
		}
		block.push_back(L'\0');
		return block;
	}


	std::string FormatWindowsProcessError(DWORD code)
	{
		LPWSTR raw = nullptr;

		const DWORD length = FormatMessageW(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		    nullptr,
		    code,
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		    reinterpret_cast<LPWSTR>(&raw),
		    0,
		    nullptr);

		std::string message = length != 0 && raw != nullptr ? WideToUtf8(std::wstring_view(raw, length)) : "Unknown Windows error";
		if (raw != nullptr)
		{
			LocalFree(raw);
		}
		const std::size_t lastContent = message.find_last_not_of(" \t\r\n");
		message.erase(lastContent == std::string::npos ? 0 : lastContent + 1);
		return message;
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

	bool LaunchOwnedWindowsProcess(
	    const Process::ChildProcessRequest& request,
	    const std::vector<Process::EnvironmentOverride>& environmentOverrides,
	    HANDLE outputHandle,
	    Process::ChildProcessResult& result,
	    WindowsOwnedProcess& outProcess)
	{
		STARTUPINFOW startup{};
		startup.cb = sizeof(startup);
		startup.dwFlags = STARTF_USESTDHANDLES;
		startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startup.hStdOutput = outputHandle;
		startup.hStdError = outputHandle;
		PROCESS_INFORMATION information{};
		std::wstring commandLine = BuildCommandLine(request);
		std::wstring workingDirectory = request.WorkingDirectory.wstring();
		std::vector<wchar_t> environment = BuildEnvironment(environmentOverrides);
		if (!CreateProcessW(
		        nullptr,
		        commandLine.data(),
		        nullptr,
		        nullptr,
		        TRUE,
		        CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT | CREATE_SUSPENDED,
		        environment.empty() ? nullptr : environment.data(),
		        workingDirectory.empty() ? nullptr : workingDirectory.c_str(),
		        &startup,
		        &information))
		{
			const DWORD startError = GetLastError();
			result.StartFailure = ClassifyStartFailure(startError);
			result.FailureReason = "Failed to launch child process: " + FormatWindowsProcessError(startError);
			return false;
		}

		result.Launched = true;
		result.ProcessId = information.dwProcessId;
		outProcess.Process.Reset(information.hProcess);
		WindowsProcessHandle processThread(information.hThread);
		outProcess.Job.Reset(CreateJobObjectW(nullptr, nullptr));
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobLimits{};
		jobLimits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		if (!outProcess.Job || !SetInformationJobObject(outProcess.Job.Get(), JobObjectExtendedLimitInformation, &jobLimits, static_cast<DWORD>(sizeof(jobLimits)))
		    || !AssignProcessToJobObject(outProcess.Job.Get(), outProcess.Process.Get()))
		{
			const DWORD ownershipError = GetLastError();
			TerminateProcess(outProcess.Process.Get(), 1);
			WaitForSingleObject(outProcess.Process.Get(), ProcessTerminationTimeoutMilliseconds);
			result.FailureReason = "Failed to establish child process tree ownership: " + FormatWindowsProcessError(ownershipError);
			return false;
		}
		if (ResumeThread(processThread.Get()) == static_cast<DWORD>(-1))
		{
			const DWORD resumeError = GetLastError();
			TerminateOwnedWindowsProcess(outProcess);
			result.FailureReason = "Failed to start child process: " + FormatWindowsProcessError(resumeError);
			return false;
		}
		return true;
	}

}
#endif
