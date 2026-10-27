#pragma once

struct LevelDesc;
struct GameWorldResourceStores;

namespace Assets
{
	struct SceneLoadPackage;
}

namespace ECS
{
	class GameWorldState;
}

void ValidateSceneLoadEntityCount(const Assets::SceneLoadPackage& package);
void InitializeSceneLoadLevel(ECS::GameWorldState& state, const LevelDesc& desc);
void PopulateStagedScene(ECS::GameWorldState& state, GameWorldResourceStores& resources, Assets::SceneLoadPackage& package);
