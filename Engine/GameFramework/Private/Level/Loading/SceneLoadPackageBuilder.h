#pragma once

namespace Assets
{
	struct SceneAssetLoadWork;
	struct SceneLoadWorkState;

	void PrepareSceneAssetForCommit(SceneAssetLoadWork& work);
	void FinalizeSceneLoadPackage(SceneLoadWorkState& state);
}
