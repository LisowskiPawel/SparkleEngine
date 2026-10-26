#include "PCH.h"

#include "LoggerBootstrap.h"

#include "Core/Public/Environment/EnvironmentVariables.h"
#include "Paths/LogPathPolicy.h"

#include <memory>
#include <mutex>
#include <string>

#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

#if defined(_WIN32)
  #include <Windows.h>
#endif

namespace Logging
{
	spdlog::level::level_enum ReadConfiguredLoggerLevel() noexcept
	{
		std::string configuredLevel;
		if (!Environment::TryGetVariable("SPARKLE_LOG_LEVEL", configuredLevel))
		{
			return spdlog::level::info;
		}

		for (char& character : configuredLevel)
		{
			if (character >= 'A' && character <= 'Z')
			{
				character = static_cast<char>(character - 'A' + 'a');
			}
		}

		const spdlog::level::level_enum configuredSpdlogLevel = spdlog::level::from_str(configuredLevel);
		return configuredSpdlogLevel == spdlog::level::off && configuredLevel != "off" ? spdlog::level::info : configuredSpdlogLevel;
	}

#if defined(_WIN32)
	class DebugOutputSink final : public spdlog::sinks::base_sink<std::mutex>
	{
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			spdlog::memory_buf_t formatted;
			spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
			formatted.push_back('\0');
			::OutputDebugStringA(formatted.data());
		}

		void flush_() override {}
	};
#endif

	std::vector<spdlog::sink_ptr> CreateLoggerSinks(std::optional<std::filesystem::path>& activeLogFilePath)
	{
		std::vector<spdlog::sink_ptr> sinks;
		activeLogFilePath.reset();

		auto stderrSink = std::make_shared<spdlog::sinks::stderr_sink_mt>();
		stderrSink->set_pattern("[%n] [%l] %s:%# %v");
		sinks.push_back(stderrSink);

#if defined(_WIN32)
		auto debuggerSink = std::make_shared<DebugOutputSink>();
		debuggerSink->set_pattern("[%n] [%l] %s:%# %v");
		sinks.push_back(debuggerSink);
#endif

		try
		{
			std::string configuredFile;
			Environment::TryGetVariable("SPARKLE_LOG_FILE", configuredFile);
			const std::filesystem::path logPath = Paths::Private::ResolveBootstrapLogFile(configuredFile, Paths::Private::LogParentDirectoryPolicy::EnsureExists);

			auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath.string(), 5 * 1024 * 1024, 2);
			fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %s:%# %v");
			sinks.push_back(fileSink);
			activeLogFilePath = logPath;
		}
		catch (...)
		{
			return sinks;
		}

		return sinks;
	}

}
