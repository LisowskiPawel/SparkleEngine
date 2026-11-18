#include "PCH.h"
#include "Logger.h"

#include "LoggerBootstrap.h"

#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Logging
{
	static constexpr std::string_view kCoreLoggerName = "SparkleCore";
	static std::shared_ptr<spdlog::logger> GetOrCreateLogger(std::string_view name) noexcept;

	std::shared_ptr<spdlog::logger> LogCategory::GetLogger() const noexcept
	{
		return GetOrCreateLogger(m_name);
	}

	std::shared_ptr<spdlog::logger> LogCategory::operator->() const noexcept
	{
		return GetLogger();
	}

	LogCategory::operator std::shared_ptr<spdlog::logger>() const noexcept
	{
		return GetLogger();
	}

	using LoggerMap = std::unordered_map<std::string, std::shared_ptr<spdlog::logger>>;

	struct LoggerRegistry final
	{
		std::mutex Mutex;
		LoggerMap NamedLoggers;
		std::vector<spdlog::sink_ptr> SharedSinks;
		spdlog::level::level_enum Level = spdlog::level::info;
		bool bInitialized = false;
		std::optional<std::filesystem::path> ActiveLogFilePath;
	};

	static LoggerRegistry& GetLoggerRegistry() noexcept
	{
		static LoggerRegistry registry;
		return registry;
	}

	static std::shared_ptr<spdlog::logger> CreateLoggerLocked(std::string_view name)
	{
		const LoggerRegistry& registry = GetLoggerRegistry();
		auto logger = std::make_shared<spdlog::logger>(std::string(name), registry.SharedSinks.begin(), registry.SharedSinks.end());
		logger->set_level(registry.Level);
		logger->flush_on(spdlog::level::err);
		return logger;
	}

	static void EnsureInitializedLocked()
	{
		LoggerRegistry& registry = GetLoggerRegistry();
		if (registry.bInitialized)
		{
			return;
		}

		registry.SharedSinks = CreateLoggerSinks(registry.ActiveLogFilePath);
		registry.Level = ReadConfiguredLoggerLevel();
		auto coreLogger = CreateLoggerLocked(kCoreLoggerName);
		registry.NamedLoggers.emplace(std::string(kCoreLoggerName), coreLogger);
		spdlog::set_default_logger(coreLogger);
		registry.bInitialized = true;
	}

	void Initialize() noexcept
	{
		try
		{
			std::lock_guard<std::mutex> lock(GetLoggerRegistry().Mutex);
			EnsureInitializedLocked();
		}
		catch (...)
		{
			return;
		}
	}

	std::optional<std::filesystem::path> GetActiveLogFilePath() noexcept
	{
		try
		{
			LoggerRegistry& registry = GetLoggerRegistry();
			std::lock_guard<std::mutex> lock(registry.Mutex);
			EnsureInitializedLocked();
			return registry.ActiveLogFilePath;
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

	static std::shared_ptr<spdlog::logger> GetOrCreateLogger(std::string_view name) noexcept
	{
		try
		{
			LoggerRegistry& registry = GetLoggerRegistry();
			std::lock_guard<std::mutex> lock(registry.Mutex);
			EnsureInitializedLocked();
			const std::string loggerName(name);
			const auto existing = registry.NamedLoggers.find(loggerName);
			if (existing != registry.NamedLoggers.end())
			{
				return existing->second;
			}

			auto logger = CreateLoggerLocked(name);
			registry.NamedLoggers.emplace(loggerName, logger);
			return logger;
		}
		catch (...)
		{
			return spdlog::default_logger();
		}
	}

}
