#pragma once

#include "Types/ImportedScene.h"

#include <filesystem>

struct cgltf_data;
struct cgltf_material;

class GltfMaterialImporter final
{
public:
	static void ImportMaterials(const cgltf_data& data, const std::filesystem::path& sourceDirectory, ImportedScene& importedScene);

private:
	static ImportedMaterial ExtractMaterial(const cgltf_material& material, ImportedMaterialIndex materialIndex, const std::filesystem::path& sourceDirectory);
};
