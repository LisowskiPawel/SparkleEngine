#include "PCH.h"

#include "FileSystemPathState.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Paths/UserStatePaths.h"

namespace Filesystem
{
	static void DiscoverWorkspaceAssetRoots(FileSystemPathState& state)
	{
		if (const auto engineRoot = Filesystem::DiscoverEngineRoot())
		{
			state.enginePath = *engineRoot;
			state.engineAssetsPath = state.enginePath / "Assets";
		}
		if (const auto projectRoot = Filesystem::DiscoverProjectRoot())
		{
			state.projectPath = *projectRoot;
			state.projectAssetsPath = state.projectPath / "Assets";
		}
	}

	void DiscoverFileSystemPathRoots(FileSystemPathState& state)
	{
		state.workingDirectory = std::filesystem::current_path();
		state.executableDirectory = Filesystem::GetExecutableDirectory();

		state.workspacePath = Filesystem::ResolveWorkspaceRootPath();
		state.productUserStatePaths = ResolveCurrentProductUserStatePaths();
		DiscoverWorkspaceAssetRoots(state);

		state.projectPath = Paths::Normalize(state.projectPath);
		state.projectAssetsPath = Paths::Normalize(state.projectAssetsPath);
		state.enginePath = Paths::Normalize(state.enginePath);
		state.engineAssetsPath = Paths::Normalize(state.engineAssetsPath);
	}

}
