#include "PCH.h"

#include "AssetPathState.h"

namespace Filesystem::Private
{
	static AssetPathState CreateAssetPathState()
	{
		AssetPathState state;
		DiscoverAssetPathRoots(state);
		RebuildProjectPaths(state);
		return state;
	}

	void RebuildProjectPaths(AssetPathState& state)
	{
		RebuildCookedPaths(state);
		RebuildTypedPaths(state);
		MaterializeOutputDirectories(state);
		ValidateConfiguredPaths(state);
	}

	AssetPathState& GetAssetPathState()
	{
		static AssetPathState state = CreateAssetPathState();
		return state;
	}
}
