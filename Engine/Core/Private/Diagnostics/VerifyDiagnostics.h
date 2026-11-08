#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include <spdlog/common.h>

namespace spdlog
{
	class logger;
}

namespace Diagnostics
{
	std::string BuildHResultRecord(std::int32_t result, const char* expression);
	void WriteRecord(const std::shared_ptr<spdlog::logger>& logger, const char* file, std::uint32_t line, std::string_view message, spdlog::level::level_enum level) noexcept;
	void WriteFallback(std::string_view record) noexcept;
	void BreakAttachedDebugger() noexcept;
}
