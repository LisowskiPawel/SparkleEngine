#pragma once

#include "Types/ImportedSceneIndices.h"

#include <filesystem>

struct cgltf_material;
struct ImportedMaterial;

namespace GltfMaterialTextureMapper
{
	void Apply(const cgltf_material& material, ImportedMaterialIndex materialIndex, const std::filesystem::path& sourceDirectory, ImportedMaterial& importedMaterial);
}
