#include "PCH.h"
#include "Logger.h"

#include "LoggerBootstrap.h"

#include <atomic>
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
		std::atomic<bool> bInitialized{false};
		std::optional<std::filesystem::path> ActiveLogFilePath;
	};

	static LoggerRegistry& GetLoggerRegistry() noexcept
	{
		static LoggerRegistry registry;
		return registry;
	}

	static void ApplyLoggerSettingsLocked(const std::shared_ptr<spdlog::logger>& logger) noexcept
	{
		if (!logger)
		{
			return;
		}

		logger->set_level(GetLoggerRegistry().Level);
		logger->flush_on(spdlog::level::err);
	}

	static std::shared_ptr<spdlog::logger> CreateLoggerLocked(std::string_view name)
	{
		const std::vector<spdlog::sink_ptr>& sharedSinks = GetLoggerRegistry().SharedSinks;
		auto logger = std::make_shared<spdlog::logger>(std::string(name), sharedSinks.begin(), sharedSinks.end());
		ApplyLoggerSettingsLocked(logger);
		return logger;
	}

	static void EnsureInitializedLocked()
	{
		LoggerRegistry& registry = GetLoggerRegistry();
		if (registry.bInitialized.load(std::memory_order_acquire))
		{
			return;
		}

		registry.SharedSinks = CreateLoggerSinks(registry.ActiveLogFilePath);
		registry.Level = ReadConfiguredLoggerLevel();
		auto coreLogger = CreateLoggerLocked(kCoreLoggerName);
		registry.NamedLoggers.emplace(std::string(kCoreLoggerName), coreLogger);
		spdlog::set_default_logger(coreLogger);
		registry.bInitialized.store(true, std::memory_order_release);
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

	bool IsInitialized() noexcept
	{
		return GetLoggerRegistry().bInitialized.load(std::memory_order_acquire);
	}

	std::optional<std::filesystem::path> GetActiveLogFilePath() noexcept
	{
		Initialize();

		try
		{
			LoggerRegistry& registry = GetLoggerRegistry();
			std::lock_guard<std::mutex> lock(registry.Mutex);
			return registry.ActiveLogFilePath;
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

	static std::shared_ptr<spdlog::logger> GetOrCreateLogger(std::string_view name) noexcept
	{
		Initialize();

		LoggerRegistry& registry = GetLoggerRegistry();
		// Failed bootstrap can retry sink construction; protect the borrowed sinks through creation and publication.
		std::lock_guard<std::mutex> lock(registry.Mutex);
		auto& namedLoggers = registry.NamedLoggers;
		const auto existing = namedLoggers.find(std::string(name));
		if (existing != namedLoggers.end())
		{
			return existing->second;
		}

		std::shared_ptr<spdlog::logger> logger;
		try
		{
			logger = CreateLoggerLocked(name);
		}
		catch (...)
		{
			logger = spdlog::default_logger();
		}

		namedLoggers.emplace(std::string(name), logger);

		if (logger)
		{
			return logger;
		}

		const auto coreLoggerIt = namedLoggers.find(std::string(kCoreLoggerName));
		if (coreLoggerIt != namedLoggers.end())
		{
			return coreLoggerIt->second;
		}

		return nullptr;
	}

}
