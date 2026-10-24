#pragma once

#include "Core/Public/Process/ChildProcess.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	class IProcessRunner
	{
	public:
		virtual ~IProcessRunner() = default;

		IProcessRunner(const IProcessRunner&) = delete;
		IProcessRunner& operator=(const IProcessRunner&) = delete;
		IProcessRunner(IProcessRunner&&) = delete;
		IProcessRunner& operator=(IProcessRunner&&) = delete;

		virtual Process::ChildProcessResult Run(const Process::ChildProcessRequest& request) = 0;

	protected:
		IProcessRunner() = default;
	};

	class NativeProcessRunner final : public IProcessRunner
	{
	public:
		Process::ChildProcessResult Run(const Process::ChildProcessRequest& request) override;
	};

	void AppendProcessOutputCallback(Process::ChildProcessRequest& request, Process::ChildProcessOutputCallback callback);
	std::filesystem::path ReadProcessOutputPath(std::string_view output, std::string_view prefix);
	std::string BuildDisplayCommandLine(const std::filesystem::path& executablePath, const std::vector<std::string>& arguments);
}
