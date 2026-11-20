#include "PCH.h"

#include "Paths/FileSystemDiscovery.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"

#include <system_error>
#include <utility>

namespace Filesystem
{
	std::optional<std::filesystem::path> FindAncestorWithMarker(const std::filesystem::path& startDirectory, std::string_view markerFileName, std::uint32_t maxDepth)
	{
		if (startDirectory.empty() || markerFileName.empty())
		{
			return std::nullopt;
		}

		std::error_code errorCode;
		std::filesystem::path currentDirectory = std::filesystem::weakly_canonical(startDirectory, errorCode);
		if (errorCode)
		{
			currentDirectory = startDirectory;
		}

		for (std::uint32_t depth = 0; depth < maxDepth && !currentDirectory.empty(); ++depth)
		{
			if (std::filesystem::exists(currentDirectory / markerFileName, errorCode))
			{
				return currentDirectory;
			}

			std::filesystem::path parentDirectory = currentDirectory.parent_path();
			if (parentDirectory == currentDirectory)
			{
				break;
			}
			currentDirectory = std::move(parentDirectory);
		}
		return std::nullopt;
	}

	std::optional<std::filesystem::path> FindProcessAncestorWithMarker(std::string_view marker)
	{
		const auto executableDirectory = GetExecutableDirectory();
		if (const auto executableRoot = FindAncestorWithMarker(executableDirectory, marker))
		{
			return Paths::Normalize(*executableRoot);
		}

		std::error_code errorCode;
		{
			const auto workingDirectory = std::filesystem::current_path(errorCode);
			const auto workingRoot = FindAncestorWithMarker(workingDirectory, marker);

			if (workingRoot && !errorCode)
			{
				return Paths::Normalize(*workingRoot);
			}
		}
		return std::nullopt;
	}

}
