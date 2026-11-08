#include "PCH.h"

#include "FileSystemPathState.h"

namespace Filesystem
{
	static FileSystemPathState CreateFileSystemPathState()
	{
		FileSystemPathState state;
		DiscoverFileSystemPathRoots(state);
		RebuildProjectPaths(state);
		return state;
	}

	void RebuildProjectPaths(FileSystemPathState& state)
	{
		RebuildCookedAssetPaths(state);
		ValidateConfiguredPaths(state);
	}

	FileSystemPathState& GetFileSystemPathState()
	{
		static FileSystemPathState state = CreateFileSystemPathState();
		return state;
	}
}
