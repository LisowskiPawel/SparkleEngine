#pragma once

#include "Types/ImportedScene.h"

#include <assimp/scene.h>

class FbxCameraImporter final
{
public:
	static void ImportCameras(const aiScene& scene, float sourceMetersPerUnit, ImportedScene& importedScene);

private:
	static float ResolveFovYRadians(const aiCamera& camera) noexcept;
};
