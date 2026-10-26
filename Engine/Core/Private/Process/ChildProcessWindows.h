#pragma once

#include "Process/ChildProcess.h"

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <string>
#include <string_view>
#include <vector>

namespace Process
{
	class WindowsProcessHandle final
	{
	public:
		WindowsProcessHandle() noexcept = default;
		explicit WindowsProcessHandle(HANDLE handle) noexcept;
		~WindowsProcessHandle();
		WindowsProcessHandle(const WindowsProcessHandle&) = delete;
		WindowsProcessHandle& operator=(const WindowsProcessHandle&) = delete;
		WindowsProcessHandle(WindowsProcessHandle&& other) noexcept;
		WindowsProcessHandle& operator=(WindowsProcessHandle&& other) noexcept;
		HANDLE Get() const noexcept { return m_handle; }
		explicit operator bool() const noexcept;
		void Reset(HANDLE handle = nullptr) noexcept;
	private:
		HANDLE m_handle = nullptr;
	};

	struct WindowsOwnedProcess final
	{
		WindowsProcessHandle Process;
		WindowsProcessHandle Job;
	};

	std::string WideToUtf8(std::wstring_view text);
	std::string FormatWindowsProcessError(DWORD code);
	void TerminateOwnedWindowsProcess(const WindowsOwnedProcess& process) noexcept;
	bool PrepareChildProcessLogDirectory(const std::filesystem::path& logPath, ChildProcessResult& result);
	bool LaunchOwnedWindowsProcess(const ChildProcessRequest& request, const std::vector<EnvironmentOverride>& environmentOverrides, HANDLE outputHandle, ChildProcessResult& result, WindowsOwnedProcess& outProcess);
	ChildProcessResult RunWindowsChildProcess(const ChildProcessRequest& request);
	ChildProcessResult RunWindowsChildProcessUntilReady(const ChildProcessRequest& request);
	ChildProcessResult RunWindowsChildProcessToExit(const ChildProcessRequest& request);
}
#endif
