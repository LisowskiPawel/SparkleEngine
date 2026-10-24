#pragma once

#include "ShaderRecook/ShaderRecookRequest.h"
#include "Core/Public/Process/ChildProcess.h"

#include <filesystem>
#include <stop_token>
#include <string>
#include <string_view>
#include <vector>

inline constexpr int kShaderCompilerNoWorkExitCode = 2;

struct ShaderCompilerProcessResult final
{
	Process::ChildProcessResult Execution;
	std::string CommandLine;

	bool Succeeded() const noexcept { return Execution.Succeeded(); }

	bool NoWork() const noexcept { return Execution.Launched && !Execution.Cancelled && Execution.ExitCode == kShaderCompilerNoWorkExitCode && Execution.FailureReason.empty(); }

	bool SettledSuccessfully() const noexcept { return Succeeded() || NoWork(); }

	std::string TakeOutput();
};

class ShaderCompilerProcess final
{
public:
	ShaderCompilerProcess() = delete;

	static ShaderCompilerProcessResult RunCook(const ShaderRecookRequest& request, std::stop_token cancellation = {}) noexcept;
	static ShaderCompilerProcessResult RunToolCommand(std::string_view command) noexcept;

private:
	static std::filesystem::path ResolveExecutable() noexcept;
	static std::filesystem::path ResolveProjectDirectory() noexcept;
	static ShaderCompilerProcessResult RunCommand(const std::filesystem::path& executablePath, const std::filesystem::path& workingDirectory, std::vector<std::string> arguments) noexcept;
};
