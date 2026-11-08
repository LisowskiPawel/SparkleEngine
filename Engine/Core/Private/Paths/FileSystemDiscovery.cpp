#include "PCH.h"

#include "Paths/FileSystemDiscovery.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"

#include <system_error>

namespace Filesystem
{
	std::optional<std::filesystem::path> DiscoverWorkspaceRoot()
	{
		return FindProcessAncestorWithMarker(kWorkspaceMarker);
	}

	std::optional<std::filesystem::path> DiscoverEngineRoot()
	{
		if (const auto processRoot = FindProcessAncestorWithMarker(kEngineMarker))
		{
			return processRoot;
		}
		std::error_code errorCode;
		if (const auto workspaceRoot = DiscoverWorkspaceRoot())
		{
			const std::filesystem::path enginePath = *workspaceRoot / "engine";
			if (std::filesystem::exists(enginePath / kEngineMarker, errorCode))
			{
				return Paths::Normalize(enginePath);
			}
		}
		return std::nullopt;
	}

	std::optional<std::filesystem::path> DiscoverProjectRoot()
	{
		if (const auto processRoot = FindProcessAncestorWithMarker(kProjectMarker))
		{
			return processRoot;
		}
		if (const auto workspaceProjectRoot = DiscoverWorkspaceProjectRoot())
		{
			return Paths::Normalize(*workspaceProjectRoot);
		}

		return std::nullopt;
	}

	std::filesystem::path ResolveArtifactRootPath(const std::filesystem::path& workspaceRoot)
	{
		return Paths::Normalize(workspaceRoot / "artifacts" / SPARKLE_ARTIFACT_VARIANT_NAME);
	}

	std::filesystem::path ResolveWorkspaceRootPath()
	{
		if (const auto workspaceRoot = DiscoverWorkspaceRoot())
		{
			return Paths::Normalize(*workspaceRoot);
		}
		if (const auto engineRoot = DiscoverEngineRoot())
		{
			return Paths::Normalize(engineRoot->parent_path());
		}

		std::error_code errorCode;
		const std::filesystem::path workingDirectory = std::filesystem::current_path(errorCode);
		return !workingDirectory.empty() && !errorCode ? Paths::Normalize(workingDirectory) : Paths::Normalize(GetExecutableDirectory());
	}

}
