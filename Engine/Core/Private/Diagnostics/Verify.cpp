#include "PCH.h"
#include "Verify.h"

#include "Logger.h"

#include <array>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32)
  #include <Windows.h>
#endif

namespace Diagnostics
{
	static bool TryLogFatal(const std::shared_ptr<spdlog::logger>& logger, const char* file, std::uint32_t line, std::string_view message) noexcept
	{
		try
		{
			if (logger)
			{
				logger->log(spdlog::source_loc{file, static_cast<int>(line), ""}, spdlog::level::critical, message);
				logger->flush();
				return true;
			}
		}
		catch (...)
		{
		}
		return false;
	}

	[[noreturn]] void Fatal(const std::shared_ptr<spdlog::logger>& logger, const char* file, std::uint32_t line, std::string_view message) noexcept
	{
		if (!TryLogFatal(logger, file, line, message))
		{
			std::fprintf(stderr, "%s:%u: ", file != nullptr ? file : "", static_cast<unsigned>(line));
			std::fwrite(message.data(), 1, message.size(), stderr);
			std::fputc('\n', stderr);
			std::fflush(stderr);
		}
#if defined(_WIN32) && !defined(NDEBUG)
		if (::IsDebuggerPresent())
		{
			::DebugBreak();
		}
#endif
		std::abort();
	}

	[[noreturn]] void Fatal(const Logging::LogCategory& category, const char* file, std::uint32_t line, std::string_view message) noexcept
	{
		Fatal(category.GetLogger(), file, line, message);
	}

	[[noreturn]] void CheckHResult(std::int32_t result, const char* expression, const char* file, std::uint32_t line) noexcept
	{
		std::array<char, 512> systemMessage{};
#if defined(_WIN32)
		::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, static_cast<DWORD>(result), 0, systemMessage.data(), static_cast<DWORD>(systemMessage.size()), nullptr);
#endif
		std::string_view suffix(systemMessage.data());
		while (!suffix.empty() && (suffix.back() == '\r' || suffix.back() == '\n'))
		{
			suffix.remove_suffix(1);
		}

		std::array<char, 1024> record{};
		std::snprintf(
		    record.data(),
		    record.size(),
		    "HRESULT 0x%08X from %s: %.*s",
		    static_cast<unsigned>(result),
		    expression != nullptr ? expression : "",
		    static_cast<int>(suffix.size()),
		    suffix.data());
		SPARKLE_DEFINE_LOG_CATEGORY_STATIC(logger, "Verify");
		Fatal(logger, file, line, record.data());
	}
}
