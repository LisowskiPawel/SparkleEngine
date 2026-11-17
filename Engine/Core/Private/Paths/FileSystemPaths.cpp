#include "PCH.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#include "FileSystemPathState.h"
#include "Core/Public/Paths/PathUtils.h"

namespace Filesystem
{
	const std::filesystem::path& GetWorkspaceRootPath()
	{
		return GetFileSystemPathState().workspacePath;
	}

	const std::filesystem::path& GetProjectPath()
	{
		return GetFileSystemPathState().projectPath;
	}

	const std::filesystem::path& GetEnginePath()
	{
		return GetFileSystemPathState().enginePath;
	}

	void ConfigureProjectRoot(const std::filesystem::path& projectRoot)
	{
		FileSystemPathState& state = GetFileSystemPathState();
		state.projectPath = Paths::Normalize(projectRoot);
		RebuildProjectPaths(state);
	}
}
