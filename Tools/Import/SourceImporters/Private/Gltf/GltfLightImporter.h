#pragma once

#include "Types/ImportedScene.h"

struct cgltf_data;

class GltfLightImporter final
{
public:
	static void ImportLights(const cgltf_data& data, ImportedScene& importedScene);
};
