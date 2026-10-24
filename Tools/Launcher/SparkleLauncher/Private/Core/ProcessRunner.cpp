#include "SparkleLauncher/ProcessRunner.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Process/CommandLineUtils.h"
#include "Core/Public/Json/JsonReader.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace SparkleLauncher
{
	Process::ChildProcessResult NativeProcessRunner::Run(const Process::ChildProcessRequest& request)
	{
		return Process::ChildProcess::Run(request);
	}

	void AppendProcessOutputCallback(Process::ChildProcessRequest& request, Process::ChildProcessOutputCallback callback)
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

		Process::ChildProcessOutputCallback existingCallback = std::move(request.OutputCallback);

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
		commandLine << CommandLine::QuotePath(executablePath);
		for (const std::string& argument : arguments)
		{
			commandLine << ' ' << CommandLine::QuoteArgument(argument);
		}
		return commandLine.str();
	}
}
