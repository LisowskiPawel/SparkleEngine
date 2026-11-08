#include "Process/ChildProcessWindowsPipe.h"

#include <atomic>
#include <fstream>
#include <string>

#if defined(_WIN32)
namespace Process
{
	static void ConsumeOutput(ChildProcessResult& result, const ChildProcessOutputCallback& outputCallback, std::ofstream& log, const char* data, std::size_t size)
	{
		result.CapturedOutput.append(data, size);
		if (outputCallback)
		{
			outputCallback(std::string_view(data, size));
		}
		if (log.is_open())
		{
			log.write(data, static_cast<std::streamsize>(size));
		}
	}

	bool CreateChildOutputPipe(WindowsProcessHandle& readPipe, WindowsProcessHandle& writePipe, ChildProcessResult& result)
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

	WindowsChildOutputReader::WindowsChildOutputReader(HANDLE pipe) :
	    m_pipe(pipe),
	    m_readEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr))
	{
		m_operation.hEvent = m_readEvent.Get();
	}

	WindowsChildOutputReader::~WindowsChildOutputReader()
	{
		if (m_bPending)
		{
			CancelIoEx(m_pipe, &m_operation);
			DWORD bytesRead = 0;
			GetOverlappedResult(m_pipe, &m_operation, &bytesRead, TRUE);
		}
	}

	bool WindowsChildOutputReader::ReadAvailable(const ChildProcessOutputCallback& outputCallback, std::ofstream& log, ChildProcessResult& result)
	{
		if (m_bClosed || m_bPending)
		{
			return false;
		}
		ResetEvent(m_readEvent.Get());
		DWORD bytesRead = 0;
		if (ReadFile(m_pipe, m_buffer.data(), static_cast<DWORD>(m_buffer.size()), &bytesRead, &m_operation))
		{
			if (bytesRead == 0)
			{
				m_bClosed = true;
			}
			else
			{
				ConsumeOutput(result, outputCallback, log, m_buffer.data(), bytesRead);
			}
			return true;
		}
		const DWORD readError = GetLastError();
		if (readError == ERROR_IO_PENDING)
		{
			m_bPending = true;
		}
		else if (readError == ERROR_BROKEN_PIPE)
		{
			m_bClosed = true;
		}
		else
		{
			result.FailureReason = "Failed while reading child output: " + FormatWindowsProcessError(readError);
			m_bClosed = true;
		}
		return false;
	}

	void WindowsChildOutputReader::CompleteRead(const ChildProcessOutputCallback& outputCallback, std::ofstream& log, ChildProcessResult& result)
	{
		DWORD bytesRead = 0;
		m_bPending = false;
		if (GetOverlappedResult(m_pipe, &m_operation, &bytesRead, FALSE) && bytesRead != 0)
		{
			ConsumeOutput(result, outputCallback, log, m_buffer.data(), bytesRead);
		}
		else if (GetLastError() == ERROR_BROKEN_PIPE || bytesRead == 0)
		{
			m_bClosed = true;
		}
		else
		{
			result.FailureReason = "Failed while completing child output read: " + FormatWindowsProcessError(GetLastError());
			m_bClosed = true;
		}
	}
}
#endif
