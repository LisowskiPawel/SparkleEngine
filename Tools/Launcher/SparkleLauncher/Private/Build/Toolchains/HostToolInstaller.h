#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "Core/Public/Process/ChildProcess.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace SparkleLauncher
{
	struct HostToolInstallerDefinition final
	{
		std::string DisplayName;
		std::string InstallEffect;
	};

	const HostToolInstallerDefinition* FindHostToolInstaller(std::string_view toolId);
	bool CanInstallHostTool(std::string_view toolId, const BuildToolchainStatus& toolchain);
	std::optional<Process::ChildProcessRequest> BuildHostToolInstallRequest(
	    std::string_view toolId,
	    const BuildToolchainStatus& toolchain,
	    const std::filesystem::path& workspaceRoot,
	    std::string_view operationId,
	    std::string& errorMessage);
}
