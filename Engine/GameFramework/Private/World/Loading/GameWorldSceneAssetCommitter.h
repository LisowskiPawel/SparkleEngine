#pragma once

struct SceneAssetPayload;
struct GameWorldResourceStores;

namespace ECS
{
	class GameWorldState;
}

void CommitSceneAssetPayload(ECS::GameWorldState& state, GameWorldResourceStores& resources, SceneAssetPayload&& sceneAssetPayload);
