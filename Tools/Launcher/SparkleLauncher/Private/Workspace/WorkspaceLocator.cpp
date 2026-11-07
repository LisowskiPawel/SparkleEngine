#include "SparkleLauncher/WorkspaceLocator.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"

#include <system_error>

namespace SparkleLauncher
{
	bool IsWorkspaceRoot(const std::filesystem::path& path)
	{
		if (path.empty())
		{
			return false;
		}

		std::error_code errorCode;
		const std::filesystem::path enginePath = path / "Engine";
		const bool bHasWorkspaceMarker = std::filesystem::exists(path / std::string(Filesystem::kWorkspaceMarker), errorCode);
		errorCode.clear();
		const bool bHasEngineMarker = std::filesystem::exists(enginePath / std::string(Filesystem::kEngineMarker), errorCode);
		errorCode.clear();
		const bool bHasEngineDirectory = std::filesystem::is_directory(enginePath, errorCode);
		errorCode.clear();
		const bool bHasConfigDirectory = std::filesystem::is_directory(path / "Config", errorCode);
		errorCode.clear();
		const bool bHasProjectsDirectory = std::filesystem::is_directory(path / "Projects", errorCode);
		return bHasWorkspaceMarker && bHasEngineMarker && bHasEngineDirectory && bHasConfigDirectory && bHasProjectsDirectory;
	}

	std::optional<std::filesystem::path> TryResolveWorkspaceRoot(const std::filesystem::path& rootPath, std::string& outErrorMessage)
	{
		outErrorMessage.clear();
		const std::filesystem::path normalizedRoot = Paths::Normalize(rootPath);
		if (!IsWorkspaceRoot(normalizedRoot))
		{
			outErrorMessage = "Invalid SparkleEngine workspace root: " + rootPath.string();
			return std::nullopt;
		}

		return normalizedRoot;
	}

	std::optional<std::filesystem::path> TryFindWorkspaceRoot(const std::filesystem::path& startPath, std::string& outErrorMessage)
	{
		outErrorMessage.clear();

		std::error_code errorCode;
		std::filesystem::path current = Paths::Normalize(startPath.empty() ? std::filesystem::current_path(errorCode) : startPath);
		if (errorCode)
		{
			outErrorMessage = "Failed to resolve current directory: " + errorCode.message();
			return std::nullopt;
		}

		if (std::filesystem::is_regular_file(current, errorCode))
		{
			current = current.parent_path();
		}

		const std::optional<std::filesystem::path> workspaceRoot = Filesystem::FindAncestorWithMarker(current, Filesystem::kWorkspaceMarker);
		if (workspaceRoot)
		{
			return TryResolveWorkspaceRoot(*workspaceRoot, outErrorMessage);
		}

		outErrorMessage = "Could not find SparkleEngine workspace root marker from: " + startPath.string();
		return std::nullopt;
	}
}
