#pragma once

#include "Core/Public/Process/ChildProcess.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	Process::ChildProcessResult RunLauncherProcess(const Process::ChildProcessRequest& request);

	void AppendProcessOutputCallback(Process::ChildProcessRequest& request, Process::ChildProcessOutputCallback callback);
	std::filesystem::path ReadProcessOutputPath(std::string_view output, std::string_view prefix);
	std::string BuildDisplayCommandLine(const std::filesystem::path& executablePath, const std::vector<std::string>& arguments);
}
