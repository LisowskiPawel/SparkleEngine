#pragma once

#include <filesystem>

struct cgltf_data;
struct ImportedScene;

namespace GltfMaterialImporter
{
	void ImportMaterials(const cgltf_data& data, const std::filesystem::path& sourceDirectory, ImportedScene& importedScene);
}
