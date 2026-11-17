#include "PCH.h"

#include "FileSystemPathState.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"

namespace Filesystem
{
	static void DiscoverWorkspaceAssetRoots(FileSystemPathState& state)
	{
		if (const auto engineRoot = Filesystem::DiscoverEngineRoot())
		{
			state.enginePath = *engineRoot;
		}
		if (const auto projectRoot = Filesystem::DiscoverProjectRoot())
		{
			state.projectPath = *projectRoot;
		}
	}

	void DiscoverFileSystemPathRoots(FileSystemPathState& state)
	{
		state.workingDirectory = std::filesystem::current_path();
		state.executableDirectory = Filesystem::GetExecutableDirectory();

		state.workspacePath = Filesystem::ResolveWorkspaceRootPath();
		DiscoverWorkspaceAssetRoots(state);

		state.projectPath = Paths::Normalize(state.projectPath);
		state.enginePath = Paths::Normalize(state.enginePath);
	}
}
