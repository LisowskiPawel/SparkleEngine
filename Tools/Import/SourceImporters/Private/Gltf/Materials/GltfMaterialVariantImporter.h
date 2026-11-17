#pragma once

#include "Types/ImportedScene.h"

struct cgltf_data;

class GltfMaterialVariantImporter final
{
public:
	static void ImportMaterialVariants(const cgltf_data& data, ImportedScene& importedScene);
};
