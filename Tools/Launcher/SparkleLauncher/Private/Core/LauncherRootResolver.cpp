#include "LauncherRootResolver.h"

#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Paths/PathUtils.h"

namespace SparkleLauncher
{
	std::optional<std::filesystem::path> TryResolveLauncherRoot(const std::filesystem::path& requestedRoot, const std::filesystem::path& launcherDirectory, std::string& outErrorMessage)
	{
		const auto package = Filesystem::DiscoverPackageRoot(launcherDirectory);
		const auto requestedPackage = requestedRoot.empty() ? std::nullopt : Filesystem::DiscoverPackageRoot(requestedRoot);
		if (package || requestedPackage)
		{
			const auto root = package ? *package : *requestedPackage;
			if (!requestedRoot.empty() && Paths::Normalize(requestedRoot) != root)
			{
				outErrorMessage = "An installed Launcher cannot select another workspace with --root.";
				return std::nullopt;
			}
			if (!Filesystem::ValidatePackageFiles(root, outErrorMessage))
			{
				return std::nullopt;
			}
			const auto project = Filesystem::DiscoverPackageProjectRoot(root);
			const auto engine = Filesystem::DiscoverPackageEngineRoot(root);
			if (!project || !engine || !Filesystem::ResolvePackageContentRoot(root))
			{
				outErrorMessage = "Invalid package manifest paths.";
				return std::nullopt;
			}
			return root;
		}

		if (!requestedRoot.empty())
		{
			return TryOpenRepositoryRoot(requestedRoot, outErrorMessage);
		}

		std::error_code errorCode;
		const std::filesystem::path workingDirectory = std::filesystem::current_path(errorCode);
		if (!errorCode)
		{
			std::string workingDirectoryError;
			if (const std::optional<std::filesystem::path> repository = TryFindRepositoryRoot(workingDirectory, workingDirectoryError))
			{
				return repository;
			}
		}

		if (const std::optional<std::filesystem::path> repository = TryFindRepositoryRoot(launcherDirectory, outErrorMessage))
		{
			return repository;
		}

		outErrorMessage = "Sparkle repository not found from the working directory or Launcher location. Start the Launcher from a Sparkle checkout or pass --root <repo-root>.";
		return std::nullopt;
	}
}
