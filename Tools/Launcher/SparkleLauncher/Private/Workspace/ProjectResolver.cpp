#include "SparkleLauncher/ProjectResolver.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#include <system_error>

namespace SparkleLauncher
{
	std::optional<LauncherProject> ResolveLauncherProject(const std::filesystem::path& workspaceRoot, std::string& outErrorMessage)
	{
		outErrorMessage.clear();

		const std::filesystem::path projectsDirectory = workspaceRoot / "Projects";
		std::error_code errorCode;
		const bool bProjectsDirectoryExists = std::filesystem::exists(projectsDirectory, errorCode);
		if (errorCode)
		{
			outErrorMessage = "Failed to inspect workspace content directory: " + errorCode.message();
			return std::nullopt;
		}

		const bool bProjectsDirectoryIsDirectory = bProjectsDirectoryExists && std::filesystem::is_directory(projectsDirectory, errorCode);
		if (errorCode)
		{
			outErrorMessage = "Failed to inspect workspace content directory: " + errorCode.message();
			return std::nullopt;
		}

		if (!bProjectsDirectoryIsDirectory)
		{
			outErrorMessage = "Workspace content directory was not found: " + projectsDirectory.string();
			return std::nullopt;
		}

		std::filesystem::directory_iterator projectIterator(projectsDirectory, errorCode);
		if (errorCode)
		{
			outErrorMessage = "Failed to enumerate workspace content: " + errorCode.message();
			return std::nullopt;
		}

		std::optional<LauncherProject> discoveredContent;
		std::size_t contentCount = 0;
		const std::filesystem::directory_iterator endIterator;
		while (projectIterator != endIterator)
		{
			const std::filesystem::directory_entry& entry = *projectIterator;
			errorCode.clear();
			const bool bIsDirectory = entry.is_directory(errorCode);
			if (errorCode)
			{
				outErrorMessage = "Failed to inspect workspace content entry: " + errorCode.message();
				return std::nullopt;
			}

			if (bIsDirectory)
			{
				const std::filesystem::path markerPath = entry.path() / std::string(Filesystem::kProjectMarker);
				errorCode.clear();
				const bool bMarkerExists = std::filesystem::exists(markerPath, errorCode);
				if (errorCode)
				{
					outErrorMessage = "Failed to inspect workspace content marker: " + errorCode.message();
					return std::nullopt;
				}

				if (bMarkerExists)
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
				outErrorMessage = "Failed to enumerate workspace content: " + errorCode.message();
				return std::nullopt;
			}
		}

		if (contentCount != 1)
		{
			outErrorMessage = "SparkleLauncher requires exactly one workspace content root; found " + std::to_string(contentCount) + ".";
			return std::nullopt;
		}

		return discoveredContent;
	}

}
