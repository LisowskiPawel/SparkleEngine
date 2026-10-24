#include "BuildWorkspaceFailure.h"

#include "BuildWorkspaceProcessRequests.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Strings/StringUtils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>

namespace SparkleLauncher
{
	static std::string ReadLogText(const std::filesystem::path& logPath)
	{
		if (logPath.empty())
		{
			return {};
		}

		std::ifstream stream(logPath, std::ios::binary);
		if (!stream)
		{
			return {};
		}

		std::ostringstream contents;
		contents << stream.rdbuf();
		return contents.str();
	}

	static std::string ExtractCMakeFailureDetail(std::string_view text)
	{
		std::istringstream stream{std::string(text)};
		for (std::string line; std::getline(stream, line);)
		{
			const std::string trimmedLine = Strings::TrimCopy(line);
			if (!(trimmedLine.starts_with("CMake Error at ") || trimmedLine.starts_with("CMake Error:")))
			{
				continue;
			}

			// Inline errors already contain the cause; location headers introduce a following message block.
			if (trimmedLine.starts_with("CMake Error:"))
			{
				const std::string detail = Strings::TrimCopy(std::string_view(trimmedLine).substr(std::string_view("CMake Error:").size()));
				if (!detail.empty())
				{
					return detail;
				}
			}

			std::string detail;
			while (std::getline(stream, line))
			{
				const std::string candidate = Strings::TrimCopy(line);
				if (candidate.starts_with("Call Stack") || (candidate.empty() && !detail.empty()))
				{
					break;
				}
				if (candidate.empty())
				{
					continue;
				}
				if (!detail.empty())
				{
					detail.push_back(' ');
				}
				detail += candidate;
			}
			return detail.empty() ? trimmedLine : detail;
		}
		return {};
	}

	static std::string ExtractBuildFailureDetail(std::string_view text)
	{
		std::istringstream stream{std::string(text)};
		for (std::string line; std::getline(stream, line);)
		{
			const std::string trimmedLine = Strings::TrimCopy(line);
			if (trimmedLine.find(": error ") != std::string::npos || trimmedLine.find(" error C") != std::string::npos || trimmedLine.find("fatal error") != std::string::npos)
			{
				return trimmedLine;
			}
		}

		return {};
	}

	static std::string ExtractFailureDetail(const BuildWorkspaceProcessStep& step, const Process::ChildProcessResult& result)
	{
		std::string logText;
		std::string_view text = result.CapturedOutput;
		if (text.empty())
		{
			logText = ReadLogText(step.Request.LogPath);
			text = logText;
		}

		if (step.Id == "configure")
		{
			return ExtractCMakeFailureDetail(text);
		}
		if (step.Id == "build")
		{
			return ExtractBuildFailureDetail(text);
		}
		if (step.Id == "install-host-tool" && text.find("Visual Studio Installer could not continue because Visual Studio or an MSBuild process is running.") != std::string::npos)
		{
			return "Visual Studio or MSBuild is running. Close active IDEs and builds, then retry.";
		}
		if (step.Id == "install-host-tool")
		{
			static constexpr std::string_view installerFailurePrefix = "Visual Studio Installer exited with code ";
			const std::size_t failureStart = text.find(installerFailurePrefix);
			if (failureStart != std::string::npos)
			{
				const std::size_t failureEnd = text.find_first_of("\r\n", failureStart);
				return std::string(text.substr(failureStart, failureEnd - failureStart));
			}
		}
		return {};
	}

	std::string MakeBuildWorkspaceFailureSummary(const BuildWorkspaceProcessStep& step, const Process::ChildProcessResult& result)
	{
		if (!result.FailureReason.empty())
		{
			return result.FailureReason;
		}
		const std::string detail = ExtractFailureDetail(step, result);
		if (step.Id == "configure")
		{
			return detail.empty() ? "Generate build files failed." : "Generate build files failed: " + detail;
		}
		if (step.Id == "build")
		{
			return detail.empty() ? "Build targets failed." : "Build targets failed: " + detail;
		}
		if (step.Id == "install-host-tool")
		{
			return detail.empty() ? "Host tool installation failed." : "Host tool installation failed: " + detail;
		}
		return detail.empty() ? step.DisplayName + " failed." : step.DisplayName + " failed: " + detail;
	}

	std::string BuildWorkspaceRecoveryAction(const BuildWorkspaceProcessStep& step)
	{
		if (step.Id == "install-host-tool")
		{
			return "Close active Visual Studio builds, correct the first installer error in the named log, then retry Install.";
		}
		if (step.Id == "configure")
		{
			return "Correct the first configure or dependency error in the named log, then retry Generate Build Files.";
		}
		if (step.Id == "build")
		{
			return "Correct the first compiler, linker, or tool error in the named log, then retry this build.";
		}
		return "Correct the first tool error in the named log, then retry this operation.";
	}

}
