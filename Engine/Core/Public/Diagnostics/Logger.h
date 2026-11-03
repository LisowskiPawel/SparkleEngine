#pragma once

#include "Core/Public/CoreAPI.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>

#include <spdlog/spdlog.h>

namespace Logging
{
	class LogCategory final
	{
	public:
		constexpr explicit LogCategory(std::string_view name) noexcept :
		    m_name(name)
		{
		}

		SPARKLE_CORE_API std::shared_ptr<spdlog::logger> GetLogger() const noexcept;
		SPARKLE_CORE_API std::shared_ptr<spdlog::logger> operator->() const noexcept;
		SPARKLE_CORE_API operator std::shared_ptr<spdlog::logger>() const noexcept;

	private:
		std::string_view m_name;
	};

	// Acquire a named engine-owned logger and log through SPDLOG_LOGGER_* macros.
	// SparkleCore owns bootstrap, sink policy, and logger lifetime.
	SPARKLE_CORE_API void Initialize() noexcept;
	SPARKLE_CORE_API bool IsInitialized() noexcept;
	// Returns the file sink path owned by this process when file logging initialized successfully.
	SPARKLE_CORE_API std::optional<std::filesystem::path> GetActiveLogFilePath() noexcept;
}

#define SPARKLE_DECLARE_LOG_CATEGORY(category_name) extern const ::Logging::LogCategory category_name
#define SPARKLE_DEFINE_LOG_CATEGORY(category_name, logger_name) \
	const ::Logging::LogCategory category_name                  \
	{                                                           \
		logger_name                                             \
	}
#define SPARKLE_DEFINE_LOG_CATEGORY_STATIC(category_name, logger_name) \
	static constexpr ::Logging::LogCategory category_name              \
	{                                                                  \
		logger_name                                                    \
	}
