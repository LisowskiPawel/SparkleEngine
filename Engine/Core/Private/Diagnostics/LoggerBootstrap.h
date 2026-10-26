#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include <spdlog/common.h>

namespace Logging
{
	spdlog::level::level_enum ReadConfiguredLoggerLevel() noexcept;
	std::vector<spdlog::sink_ptr> CreateLoggerSinks(std::optional<std::filesystem::path>& activeLogFilePath);
}
