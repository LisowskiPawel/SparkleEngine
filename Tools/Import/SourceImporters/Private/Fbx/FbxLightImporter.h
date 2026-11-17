#pragma once

#include "Types/ImportedScene.h"

#include <assimp/scene.h>

class FbxLightImporter final
{
public:
	static void ImportLights(const aiScene& scene, float sourceMetersPerUnit, ImportedScene& importedScene);
};
