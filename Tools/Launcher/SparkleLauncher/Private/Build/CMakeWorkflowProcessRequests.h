#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "Core/Public/Process/ChildProcess.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	Process::ChildProcessRequest MakeCMakeConfigureRequest(
	    const std::filesystem::path& repositoryRoot,
	    const BuildToolchainStatus& toolchain,
	    std::string_view operationId,
	    std::string_view logFileName);

	Process::ChildProcessRequest MakeCMakeDependencySyncRequest(
	    const std::filesystem::path& repositoryRoot,
	    const BuildToolchainStatus& toolchain,
	    std::string_view operationId,
	    std::string_view sourceDependencyId,
	    std::string_view logFileName);

	Process::ChildProcessRequest MakeCMakeLauncherConfigureRequest(
	    const std::filesystem::path& repositoryRoot,
	    const BuildToolchainStatus& toolchain,
	    std::string_view operationId,
	    std::string_view logFileName);

	Process::ChildProcessRequest MakeCMakeBuildRequest(
	    const std::filesystem::path& repositoryRoot,
	    const BuildToolchainStatus& toolchain,
	    std::string_view operationId,
	    std::string_view profileName,
	    const std::vector<std::string>& targets,
	    std::string_view logFileName);

	Process::ChildProcessRequest MakeCMakeLauncherBuildRequest(
	    const std::filesystem::path& repositoryRoot,
	    const BuildToolchainStatus& toolchain,
	    std::string_view operationId,
	    std::string_view profileName,
	    std::string_view logFileName);
}
