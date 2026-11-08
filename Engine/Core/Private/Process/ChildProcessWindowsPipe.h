#pragma once

#include "Process/ChildProcessWindows.h"

#include <array>
#include <iosfwd>

#if defined(_WIN32)
namespace Process
{
	bool CreateChildOutputPipe(WindowsProcessHandle& readPipe, WindowsProcessHandle& writePipe, ChildProcessResult& result);

	class WindowsChildOutputReader final
	{
	public:
		explicit WindowsChildOutputReader(HANDLE pipe);
		~WindowsChildOutputReader();
		WindowsChildOutputReader(const WindowsChildOutputReader&) = delete;
		WindowsChildOutputReader& operator=(const WindowsChildOutputReader&) = delete;

		bool IsValid() const noexcept { return static_cast<bool>(m_readEvent); }

		bool IsClosed() const noexcept { return m_bClosed; }

		bool HasPendingRead() const noexcept { return m_bPending; }

		HANDLE GetReadEvent() const noexcept { return m_readEvent.Get(); }

		// A synchronous completion must be drained again before waiting for another event.
		bool ReadAvailable(const ChildProcessOutputCallback& outputCallback, std::ofstream& log, ChildProcessResult& result);
		void CompleteRead(const ChildProcessOutputCallback& outputCallback, std::ofstream& log, ChildProcessResult& result);

	private:
		HANDLE m_pipe;
		WindowsProcessHandle m_readEvent;
		std::array<char, 4096> m_buffer{};
		OVERLAPPED m_operation{};
		bool m_bPending = false;
		bool m_bClosed = false;
	};
}
#endif
