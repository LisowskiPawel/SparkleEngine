#include "PCH.h"
#include "Logger.h"

#include "LoggerBootstrap.h"

#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

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

	struct LoggerBootstrapState final
	{
		std::mutex Mutex;
		bool bInitialized = false;
		std::optional<std::filesystem::path> ActiveLogFilePath;
	};

	static LoggerBootstrapState& GetBootstrapState() noexcept
	{
		static LoggerBootstrapState state;
		return state;
	}

	static void EnsureInitializedLocked()
	{
		LoggerBootstrapState& state = GetBootstrapState();
		if (state.bInitialized)
		{
			return;
		}

		const auto sinks = CreateLoggerSinks(state.ActiveLogFilePath);
		auto coreLogger = std::make_shared<spdlog::logger>(std::string(kCoreLoggerName), sinks.begin(), sinks.end());
		coreLogger->set_level(ReadConfiguredLoggerLevel());
		coreLogger->flush_on(spdlog::level::err);
		spdlog::set_default_logger(coreLogger);
		state.bInitialized = true;
	}

	void Initialize() noexcept
	{
		try
		{
			std::lock_guard<std::mutex> lock(GetBootstrapState().Mutex);
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
			LoggerBootstrapState& state = GetBootstrapState();
			std::lock_guard<std::mutex> lock(state.Mutex);
			EnsureInitializedLocked();
			return state.ActiveLogFilePath;
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
			std::lock_guard<std::mutex> lock(GetBootstrapState().Mutex);
			EnsureInitializedLocked();
			const std::string loggerName(name);
			if (auto existing = spdlog::get(loggerName))
			{
				return existing;
			}

			auto logger = spdlog::default_logger()->clone(loggerName);
			spdlog::register_logger(logger);
			return logger;
		}
		catch (...)
		{
			return spdlog::default_logger();
		}
	}

}
