#pragma once

#include <filesystem>
#include <span>

struct aiScene;
struct ImportedScene;

namespace AssimpMaterialImporter
{
	void ImportMaterials(const aiScene& scene, const std::filesystem::path& sourceDirectory, std::span<const std::filesystem::path> embeddedTexturePaths, ImportedScene& importedScene);
}
