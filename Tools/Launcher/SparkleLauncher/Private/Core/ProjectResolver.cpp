#include "SparkleLauncher/ProjectResolver.h"

#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Paths/ProductPaths.h"
#include "PackageContentStore.h"

#include <system_error>

namespace SparkleLauncher
{
	std::optional<LauncherProject> ResolveLauncherProject(const std::filesystem::path& repositoryRoot, std::string& outErrorMessage)
	{
		outErrorMessage.clear();

		if (!Filesystem::IsSourceWorkspace(repositoryRoot))
		{
			if (!InitializePackageContentStore(repositoryRoot, outErrorMessage))
			{
				return std::nullopt;
			}
			const auto project = Filesystem::ResolvePackageSourceRoot(repositoryRoot);
			if (!project)
			{
				outErrorMessage = "Invalid package content-store path.";
				return std::nullopt;
			}
			return LauncherProject{project->filename().string(), *project};
		}

		const std::filesystem::path projectsDirectory = repositoryRoot / "Projects";
		std::error_code errorCode;
		const bool projectsDirectoryExists = std::filesystem::exists(projectsDirectory, errorCode);
		if (errorCode)
		{
			outErrorMessage = "Failed to inspect repository content directory: " + errorCode.message();
			return std::nullopt;
		}

		const bool projectsDirectoryIsDirectory = projectsDirectoryExists && std::filesystem::is_directory(projectsDirectory, errorCode);
		if (errorCode)
		{
			outErrorMessage = "Failed to inspect repository content directory: " + errorCode.message();
			return std::nullopt;
		}

		if (!projectsDirectoryIsDirectory)
		{
			outErrorMessage = "Repository content directory was not found: " + projectsDirectory.string();
			return std::nullopt;
		}

		std::filesystem::directory_iterator projectIterator(projectsDirectory, errorCode);
		if (errorCode)
		{
			outErrorMessage = "Failed to enumerate repository content: " + errorCode.message();
			return std::nullopt;
		}

		std::optional<LauncherProject> discoveredContent;
		std::size_t contentCount = 0;
		const std::filesystem::directory_iterator endIterator;
		while (projectIterator != endIterator)
		{
			const std::filesystem::directory_entry& entry = *projectIterator;
			errorCode.clear();
			const bool isDirectory = entry.is_directory(errorCode);
			if (errorCode)
			{
				outErrorMessage = "Failed to inspect repository content entry: " + errorCode.message();
				return std::nullopt;
			}

			if (isDirectory)
			{
				const std::filesystem::path markerPath = entry.path() / std::string(Filesystem::kProjectMarker);
				errorCode.clear();
				const bool markerExists = std::filesystem::exists(markerPath, errorCode);
				if (errorCode)
				{
					outErrorMessage = "Failed to inspect repository content marker: " + errorCode.message();
					return std::nullopt;
				}

				if (markerExists)
				{
					++contentCount;
					if (!discoveredContent)
					{
						discoveredContent = LauncherProject{
						    .Id = entry.path().filename().string(),
						    .RootPath = entry.path(),
						};
					}
				}
			}

			projectIterator.increment(errorCode);
			if (errorCode)
			{
				outErrorMessage = "Failed to enumerate repository content: " + errorCode.message();
				return std::nullopt;
			}
		}

		if (contentCount != 1)
		{
			outErrorMessage = "SparkleLauncher requires exactly one repository content root; found " + std::to_string(contentCount) + ".";
			return std::nullopt;
		}

		return discoveredContent;
	}

}
