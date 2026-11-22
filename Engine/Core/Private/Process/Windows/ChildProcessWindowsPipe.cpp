#include "Process/Windows/ChildProcessWindowsPipe.h"
#include <atomic>
#include <string>

#if defined(_WIN32)
namespace Process
{
	bool CreateChildOutputPipe(WindowsProcessHandle& readPipe, WindowsProcessHandle& writePipe, ChildProcessResult& result)
	{
		static std::atomic_uint64_t nextPipeIdentity{1};
		const DWORD processId = GetCurrentProcessId();
		const auto pipeIdentity = nextPipeIdentity.fetch_add(1, std::memory_order_relaxed);
		const std::wstring pipeName = L"\\\\.\\pipe\\Sparkle.ChildProcess." + std::to_wstring(processId) + L"." + std::to_wstring(pipeIdentity);
		const HANDLE readHandle = CreateNamedPipeW(pipeName.c_str(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 64 * 1024, 64 * 1024, 0, nullptr);
		readPipe.Reset(readHandle);
		if (!readPipe)
		{
			result.FailureReason = "Failed to create child output pipe: " + FormatWindowsProcessError(GetLastError());
			return false;
		}

		SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
		const HANDLE writeHandle = CreateFileW(pipeName.c_str(), GENERIC_WRITE, 0, &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		writePipe.Reset(writeHandle);
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
}
#endif
