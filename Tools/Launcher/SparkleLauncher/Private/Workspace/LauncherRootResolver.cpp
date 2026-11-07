#include "Workspace/LauncherRootResolver.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"

namespace SparkleLauncher
{
	std::optional<std::filesystem::path> TryResolveLauncherRoot(const std::filesystem::path& requestedRoot, const std::filesystem::path& launcherDirectory, std::string& outErrorMessage)
	{
		if (!requestedRoot.empty())
		{
			return TryResolveWorkspaceRoot(requestedRoot, outErrorMessage);
		}

		// Prefer the executable's own tree, including an extraction inside a checkout.
		const auto binaries = launcherDirectory.parent_path();
		const auto tool = binaries.parent_path();
		const auto tools = tool.parent_path();
		const auto products = tools.parent_path();
		const bool bLauncherLayoutMatchesfilename = binaries.filename() == "Binaries" && tool.filename() == "SparkleLauncher";
		const bool bLauncherLayoutMatchesfilename2 = bLauncherLayoutMatchesfilename && (tools.filename() == "Tools");
		const bool bLauncherArtifactLayoutMatches = bLauncherLayoutMatchesfilename2 && (products.filename() == "dev");
		
		if (bLauncherArtifactLayoutMatches)
		{
			auto artifacts = products.parent_path();
			if (artifacts.filename() != "artifacts")
			{
				artifacts = artifacts.parent_path();
			}
			if (artifacts.filename() == "artifacts")
			{
				return TryResolveWorkspaceRoot(artifacts.parent_path(), outErrorMessage);
			}
		}
		if (const auto root = TryFindWorkspaceRoot(launcherDirectory, outErrorMessage))
		{
			return root;
		}
		std::error_code errorCode;
		const std::filesystem::path workingDirectory = std::filesystem::current_path(errorCode);
		if (!errorCode)
		{
			std::string workingDirectoryError;
			if (const std::optional<std::filesystem::path> workspace = TryFindWorkspaceRoot(workingDirectory, workingDirectoryError))
			{
				return workspace;
			}
		}

		outErrorMessage = "Sparkle workspace not found from the working directory or Launcher location. Start the Launcher from a Sparkle workspace or pass --root <workspace-root>.";
		return std::nullopt;
	}
}
