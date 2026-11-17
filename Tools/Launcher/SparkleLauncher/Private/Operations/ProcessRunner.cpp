#include "SparkleLauncher/ProcessRunner.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Process/CommandLineUtils.h"
#include "Core/Public/Json/JsonInputAdmission.h"

#include <nlohmann/json.hpp>

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
		return Process::RunChildProcess(request);
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
		bool bSeenRecord = false;
		while (!output.empty())
		{
			const auto end = output.find('\n');
			const auto line = output.substr(0, end);
			if (line.starts_with(prefix))
			{
				const auto suffix = line.substr(prefix.size());
				const auto first = suffix.find_first_not_of(" \t\r");
				const bool bStringRoot = first != std::string_view::npos && suffix[first] == '"';
				if (bSeenRecord || !bStringRoot)
				{
					return {};
				}
				bSeenRecord = true;
				try
				{
					Json::InputAdmission admission(suffix, 32u * 1024u * 1024u);
					const auto value = nlohmann::json::parse(suffix.begin(), suffix.end());
					const auto& path = value.get_ref<const std::string&>();
					if (path.find('\0') != std::string::npos)
					{
						return {};
					}
					manifest = Strings::ToWide(std::string_view{path});
				}
				catch (const std::exception&)
				{
					return {};
				}
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
