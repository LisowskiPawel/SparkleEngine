#include "PCH.h"
#include "Verify.h"

#include "VerifyDiagnostics.h"

#include <cstdlib>

namespace Diagnostics
{
	[[noreturn]] void Fatal(const std::shared_ptr<spdlog::logger>& logger, const char* file, std::uint32_t line, std::string_view message) noexcept
	{
		WriteRecord(logger, file, line, message, spdlog::level::critical);
		BreakAttachedDebugger();
		std::abort();
	}

	[[noreturn]] void Fatal(const Logging::LogCategory& category, const char* file, std::uint32_t line, std::string_view message) noexcept
	{
		Fatal(category.GetLogger(), file, line, message);
	}

	void BreakInDebuggerIfAttached() noexcept
	{
		BreakAttachedDebugger();
	}

	[[noreturn]] void CheckHResult(std::int32_t result, const char* expression, const char* file, std::uint32_t line) noexcept
	{
		try
		{
			const std::string record = BuildHResultRecord(result, expression);
			SPARKLE_DEFINE_LOG_CATEGORY_STATIC(logger, "Verify");
			WriteRecord(logger, file, line, record, spdlog::level::critical);
		}
		catch (...)
		{
			WriteFallback("HRESULT verification failed while building its diagnostic record.");
		}
		BreakAttachedDebugger();
		std::abort();
	}
}
