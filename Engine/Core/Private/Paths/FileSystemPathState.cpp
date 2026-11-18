#include "PCH.h"

#include "FileSystemPathState.h"

namespace Filesystem
{
	static FileSystemPathState CreateFileSystemPathState()
	{
		FileSystemPathState state;
		DiscoverFileSystemPathRoots(state);
		RebuildCookedAssetPaths(state);
		return state;
	}

	FileSystemPathState& GetFileSystemPathState()
	{
		static FileSystemPathState state = CreateFileSystemPathState();
		return state;
	}
}
