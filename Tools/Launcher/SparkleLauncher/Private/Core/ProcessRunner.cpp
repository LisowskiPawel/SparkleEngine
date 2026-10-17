#include "SparkleLauncher/ProcessRunner.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Json/JsonReader.h"

#include <chrono>
#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace SparkleLauncher
{

	static std::string QuoteDisplayArgument(std::string_view argument)
	{
		if (argument.empty())
		{
			return "\"\"";
		}
		const bool needsQuotes = argument.find_first_of(" \t\"") != std::string_view::npos;
		if (!needsQuotes)
		{
			return std::string(argument);
		}
		std::string quoted(1, '"');
		for (const char character : argument)
		{
			if (character == '"')
			{
				quoted.push_back('\\');
			}
			quoted.push_back(character);
		}
		quoted.push_back('"');
		return quoted;
	}

	ProcessResult NativeProcessRunner::Run(const ProcessRequest& request)
	{
		ProcessResult result;
		result.StartTime = std::chrono::system_clock::now();

		Process::ChildProcessResult childResult = Process::ChildProcess::Run(
		    Process::ChildProcessRequest{
		        .ExecutablePath = request.ExecutablePath,
		        .Arguments = request.Arguments,
		        .WorkingDirectory = request.WorkingDirectory,
		        .Environment = request.Environment,
		        .LogPath = request.LogPath,
		        .OutputCallback = request.OutputCallback,
		        .Cancellation = request.Cancellation,
		        .ReadinessValue = request.ReadinessValue,
		        .ReadinessValidator = request.ReadinessValidator});

		result.ProcessId = childResult.ProcessId;
		result.Launched = childResult.Launched;
		result.Ready = childResult.Ready;
		result.Canceled = childResult.Cancelled;
		result.ExitCode = childResult.ExitCode;
		result.StartFailure = childResult.StartFailure;
		result.CapturedOutput = std::move(childResult.CapturedOutput);
		result.FailureReason = std::move(childResult.FailureReason);
		result.EndTime = std::chrono::system_clock::now();
		return result;
	}

	void AppendProcessOutputCallback(ProcessRequest& request, ProcessOutputCallback callback)
	{
		if (!callback)
		{
			return;
		}
		if (!request.OutputCallback)
		{
			request.OutputCallback = std::move(callback);
			return;
		}

		ProcessOutputCallback existingCallback = std::move(request.OutputCallback);

		request.OutputCallback = [existingCallback = std::move(existingCallback), callback = std::move(callback)](std::string_view output)
		{
			existingCallback(output);
			callback(output);
		};
	}

	std::filesystem::path ReadProcessOutputPath(std::string_view output, std::string_view prefix)
	{
		std::filesystem::path manifest;
		while (!output.empty())
		{
			const auto end = output.find('\n');
			const auto line = output.substr(0, end);
			if (line.starts_with(prefix))
			{
				std::string path;
				const auto json = "{\"manifest\":" + std::string(line.substr(prefix.size())) + "}";
				if (!manifest.empty() || !Json::TryReadStringProperty(json, "manifest", path))
				{
					return {};
				}
				manifest = Strings::ToWide(std::string_view{path});
			}
			if (end == std::string_view::npos)
			{
				break;
			}
			output.remove_prefix(end + 1);
		}
		return manifest;
	}

	std::string BuildDisplayCommandLine(const std::filesystem::path& executablePath, const std::vector<std::string>& arguments)
	{
		std::ostringstream commandLine;
		commandLine << QuoteDisplayArgument(Strings::ToNarrow(executablePath.wstring()));
		for (const std::string& argument : arguments)
		{
			commandLine << ' ' << QuoteDisplayArgument(argument);
		}
		return commandLine.str();
	}
}
