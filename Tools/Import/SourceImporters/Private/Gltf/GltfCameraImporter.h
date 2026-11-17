#pragma once

#include "Types/ImportedScene.h"

struct cgltf_data;

namespace GltfCameraImporter
{
	void ImportCameras(const cgltf_data& data, ImportedScene& importedScene);
}
