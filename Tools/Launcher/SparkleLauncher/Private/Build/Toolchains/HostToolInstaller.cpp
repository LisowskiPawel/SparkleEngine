#include "Build/Toolchains/HostToolInstaller.h"

#include "Core/Public/Environment/EnvironmentVariables.h"
#include "Workspace/LauncherStatePaths.h"

namespace SparkleLauncher
{
	static constexpr std::string_view kClangClToolId = "clangcl";
	static constexpr std::string_view kClangClVisualStudioCompilerComponent = "Microsoft.VisualStudio.Component.VC.Llvm.Clang";
	static constexpr std::string_view kClangClVisualStudioToolsetComponent = "Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset";

	static std::string QuotePowerShellLiteral(std::string value)
	{
		std::size_t position = 0;
		while ((position = value.find('\'', position)) != std::string::npos)
		{
			value.insert(position, 1, '\'');
			position += 2;
		}
		return "'" + value + "'";
	}

	static std::filesystem::path ResolvePowerShellPath()
	{
#if defined(_WIN32)
		std::string systemRoot;
		if (Environment::TryGetVariable("SystemRoot", systemRoot))
		{
			return std::filesystem::path(systemRoot) / "System32" / "WindowsPowerShell" / "v1.0" / "powershell.exe";
		}
		return "powershell.exe";
#else
		return "pwsh";
#endif
	}

	static bool CanInstallClangCl(const BuildToolchainStatus& toolchain)
	{
		return !toolchain.VisualStudioPath.empty() && !toolchain.VisualStudioInstallerPath.empty();
	}

	static std::optional<Process::ChildProcessRequest> BuildClangClInstallRequest(
	    const BuildToolchainStatus& toolchain,
	    const std::filesystem::path& workspaceRoot,
	    std::string_view operationId,
	    std::string& errorMessage)
	{
		const std::string installerArguments = "modify --installPath \"" + toolchain.VisualStudioPath.string() + "\" --add " + std::string(kClangClVisualStudioCompilerComponent) + " --add "
		    + std::string(kClangClVisualStudioToolsetComponent) + " --passive --norestart";

		const std::string script = "$ErrorActionPreference='Stop'; "
		                           "Write-Output 'Requesting administrator approval for the Visual Studio clang-cl component...'; "
		                           "$process = Start-Process -FilePath "
		    + QuotePowerShellLiteral(toolchain.VisualStudioInstallerPath.string()) + " -ArgumentList " + QuotePowerShellLiteral(installerArguments)
		    + " -Verb RunAs -Wait -PassThru; "
		      "if ($process.ExitCode -eq 8006) { throw 'Visual Studio Installer could not continue because Visual Studio or an MSBuild "
		      "process is running. Close active IDEs and builds, then retry.' }; "
		      "if ($process.ExitCode -ne 0) { throw ('Visual Studio Installer exited with code ' + $process.ExitCode) }; "
		      "Write-Output 'Visual Studio clang-cl components installation completed.'";

		Process::ChildProcessRequest request;
		request.ExecutablePath = ResolvePowerShellPath();
		request.Arguments = {"-NoLogo", "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass", "-Command", script};
		request.WorkingDirectory = workspaceRoot;
		request.LogPath = ResolveLauncherOperationLogPath(workspaceRoot, operationId, "InstallHostTool.txt");
		errorMessage.clear();
		return request;
	}

	const HostToolInstallerDefinition* FindHostToolInstaller(std::string_view toolId)
	{
		static const HostToolInstallerDefinition definition{
		    "clang-cl",
		    "Ask Windows for administrator approval, then add the clang-cl compiler and its MSBuild integration through Visual Studio Installer."};

		return toolId == kClangClToolId ? &definition : nullptr;
	}

	bool CanInstallHostTool(std::string_view toolId, const BuildToolchainStatus& toolchain)
	{
		return toolId == kClangClToolId && CanInstallClangCl(toolchain);
	}

	std::optional<Process::ChildProcessRequest> BuildHostToolInstallRequest(
	    std::string_view toolId,
	    const BuildToolchainStatus& toolchain,
	    const std::filesystem::path& workspaceRoot,
	    std::string_view operationId,
	    std::string& errorMessage)
	{
		const HostToolInstallerDefinition* installer = FindHostToolInstaller(toolId);
		if (installer == nullptr)
		{
			errorMessage = "No launcher installer is registered for host tool '" + std::string(toolId) + "'.";
			return std::nullopt;
		}
		if (!CanInstallClangCl(toolchain))
		{
			errorMessage = "The registered launcher installer for " + installer->DisplayName + " is not available on this machine.";
			return std::nullopt;
		}
		return BuildClangClInstallRequest(toolchain, workspaceRoot, operationId, errorMessage);
	}
}
