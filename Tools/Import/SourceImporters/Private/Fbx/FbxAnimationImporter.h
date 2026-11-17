#pragma once

#include "Types/ImportedScene.h"

#include <assimp/scene.h>

class FbxAnimationImporter final
{
public:
	static void ImportAnimations(const aiScene& scene, ImportedScene& importedScene);
};
