#include "Process/Windows/ChildProcessWindows.h"
#include "Process/Windows/ChildProcessWindowsPipe.h"

#if defined(_WIN32)
  #include <boost/asio/awaitable.hpp>
  #include <boost/asio/bind_cancellation_slot.hpp>
  #include <boost/asio/cancellation_signal.hpp>
  #include <boost/asio/co_spawn.hpp>
  #include <boost/asio/readable_pipe.hpp>
  #include <boost/asio/redirect_error.hpp>
  #include <boost/asio/use_awaitable.hpp>
  #include <boost/asio/windows/object_handle.hpp>
  #include <array>
  #include <exception>
  #include <fstream>
  #include <optional>
  #include <stop_token>

namespace Process
{
	namespace Asio = boost::asio;

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

	static Asio::awaitable<void> ReadChildOutput(const ChildProcessRequest& request, WindowsOwnedProcess& child, Asio::readable_pipe& reader, std::ofstream& log, ChildProcessResult& result)
	{
		std::array<char, 4096> bytes{};
		boost::system::error_code error;
		do
		{
			const auto size = co_await reader.async_read_some(Asio::buffer(bytes), Asio::redirect_error(Asio::use_awaitable, error));
			if (size != 0)
			{
				ConsumeOutput(result, request.OutputCallback, log, bytes.data(), size);
				if (log.is_open() && !log)
				{
					throw std::runtime_error("Failed to write child process log.");
				}
			}
		} while (!error);
		const boost::system::error_code brokenPipe(ERROR_BROKEN_PIPE, boost::system::system_category());
		const bool bPipeClosed = error == Asio::error::eof || error == brokenPipe;
		if (!bPipeClosed)
		{
			throw boost::system::system_error(error, "Failed to read child process output");
		}
		// A descendant can retain the writer after root exit; settle both before returning.
		result.ExitCode = co_await child.Process.async_wait(Asio::use_awaitable);
	}

	static void CollectChildProcessOutput(const ChildProcessRequest& request, WindowsOwnedProcess& child, WindowsProcessHandle& readPipe, std::ofstream& log, ChildProcessResult& result)
	{
		Asio::readable_pipe reader(child.Context);
		reader.assign(readPipe.get());
		readPipe.release();
		WindowsProcessHandle nativeEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr));
		if (!nativeEvent)
		{
			const DWORD eventError = GetLastError();
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = "Failed to create child cancellation event: " + FormatWindowsProcessError(eventError);
			return;
		}
		Asio::windows::object_handle event(child.Context, nativeEvent.get());
		nativeEvent.release();
		const auto signal = [handle = event.native_handle()]() noexcept
		{
			SetEvent(handle);
		};
		std::optional<std::stop_callback<decltype(signal)>> cancellation;
		cancellation.emplace(request.Cancellation, signal);
		std::exception_ptr failure;
		Asio::cancellation_signal settlementFailure;
		boost::system::error_code eventFailure;
		try
		{
			event.async_wait(
			    [&](boost::system::error_code error) noexcept
			    {
				    // Registration presence is the terminal boundary, including an already queued stop.
				    if (!cancellation)
				    {
					    return;
				    }
				    if (!error)
				    {
					    result.bCancelled = true;
					    TerminateOwnedWindowsProcess(child);
					    if (child.TerminationError != ERROR_SUCCESS)
					    {
						    settlementFailure.emit(Asio::cancellation_type::all);
					    }
				    }
				    else if (error != Asio::error::operation_aborted)
				    {
					    eventFailure = error;
					    TerminateOwnedWindowsProcess(child);
					    settlementFailure.emit(Asio::cancellation_type::all);
					    boost::system::error_code ignored;
					    reader.cancel(ignored);
				    }
			    });
			Asio::co_spawn(
			    child.Context,
			    ReadChildOutput(request, child, reader, log, result),
			    Asio::bind_cancellation_slot(
			        settlementFailure.slot(),
			        [&](std::exception_ptr error) noexcept
			        {
				        if (error)
				        {
					        if (!failure)
					        {
						        failure = error;
					        }
					        TerminateOwnedWindowsProcess(child);
				        }
				        cancellation.reset();
				        boost::system::error_code ignored;
				        event.cancel(ignored);
			        }));
			child.Context.run();
		}
		catch (...)
		{
			failure = std::current_exception();
			cancellation.reset();
			TerminateOwnedWindowsProcess(child);
			settlementFailure.emit(Asio::cancellation_type::all);
			boost::system::error_code ignored;
			reader.cancel(ignored);
			event.cancel(ignored);
			child.Context.restart();
			child.Context.run();
		}
		if (eventFailure)
		{
			result.FailureReason = "Failed to observe child cancellation: " + eventFailure.message();
		}
		else if (failure)
		{
			try
			{
				std::rethrow_exception(failure);
			}
			catch (const boost::system::system_error& error)
			{
				const bool bOperationAborted = error.code() == Asio::error::operation_aborted;
				if (!bOperationAborted || child.TerminationError == ERROR_SUCCESS)
				{
					result.FailureReason = error.what();
				}
			}
			catch (const std::exception& error)
			{
				result.FailureReason = error.what();
			}
			catch (...)
			{
				result.FailureReason = "Child process output failed.";
			}
		}
		if (log.is_open())
		{
			log.flush();
			if (!log && result.FailureReason.empty())
			{
				result.FailureReason = "Failed to write child process log.";
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
		if (!LaunchOwnedWindowsProcess(request, request.Environment, writePipe.get(), result, child))
		{
			return result;
		}
		writePipe.reset();
		try
		{
			CollectChildProcessOutput(request, child, readPipe, log, result);
		}
		catch (const std::exception& error)
		{
			TerminateOwnedWindowsProcess(child);
			result.FailureReason = error.what();
		}
		TerminateOwnedWindowsProcess(child);
		AppendWindowsProcessTerminationFailure(child, result);
		return result;
	}
}
#endif
