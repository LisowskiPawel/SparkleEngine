#pragma once

#include "Types/ImportedSceneIndices.h"

struct cgltf_material;
struct ImportedMaterial;

namespace GltfMaterialPropertyMapper
{
	void Apply(const cgltf_material& material, ImportedMaterialIndex materialIndex, ImportedMaterial& importedMaterial);
}
