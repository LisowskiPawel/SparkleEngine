#include "PCH.h"

#include "Gltf/Materials/GltfMaterialImporter.h"

#include "Gltf/Materials/GltfMaterialPropertyMapper.h"
#include "Gltf/Materials/GltfMaterialTextureMapper.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <format>
#include <utility>

class GltfMaterialFeatureReporting final
{
public:
	static void ValidateRequiredExtensions(const cgltf_data& data)
	{
		for (cgltf_size index = 0; index < data.extensions_required_count; ++index)
		{
			const std::string_view extension = data.extensions_required[index];
			if (extension == "KHR_materials_anisotropy" || extension == "KHR_materials_clearcoat" || extension == "KHR_materials_sheen" || extension == "KHR_materials_transmission"
			    || extension == "KHR_materials_volume")
			{
				throw Diagnostics::Error(std::format("Required glTF material extension '{}' has no shading implementation.", extension));
			}
		}
	}

	static void AppendFeatureName(std::string& unsupportedFeatures, std::string_view featureName)
	{
		if (!unsupportedFeatures.empty())
		{
			unsupportedFeatures += ", ";
		}

		unsupportedFeatures += featureName;
	}

	static void ValidateFeatureSupport(const cgltf_material& material, ImportedMaterialIndex materialIndex)
	{
		std::string unsupportedFeatures;
		if (material.has_pbr_specular_glossiness)
		{
			AppendFeatureName(unsupportedFeatures, "KHR_materials_pbrSpecularGlossiness");
		}
		if (material.unlit)
		{
			AppendFeatureName(unsupportedFeatures, "KHR_materials_unlit");
		}
		if (material.has_specular)
		{
			AppendFeatureName(unsupportedFeatures, "KHR_materials_specular");
		}
		if (material.has_iridescence)
		{
			AppendFeatureName(unsupportedFeatures, "KHR_materials_iridescence");
		}
		if (material.has_dispersion)
		{
			AppendFeatureName(unsupportedFeatures, "KHR_materials_dispersion");
		}

		if (!unsupportedFeatures.empty())
		{
			throw Diagnostics::Error(std::format("glTF material {} uses unsupported features [{}].", materialIndex, unsupportedFeatures));
		}
	}
};

void GltfMaterialImporter::ImportMaterials(const cgltf_data& data, const std::filesystem::path& sourceDirectory, ImportedScene& importedScene)
{
	GltfMaterialFeatureReporting::ValidateRequiredExtensions(data);
	for (cgltf_size materialIndex = 0; materialIndex < data.materials_count; ++materialIndex)
	{
		auto material = ExtractMaterial(data.materials[materialIndex], static_cast<ImportedMaterialIndex>(materialIndex), sourceDirectory);
		importedScene.materials.push_back(std::move(material));
	}
}

ImportedMaterial GltfMaterialImporter::ExtractMaterial(const cgltf_material& material, ImportedMaterialIndex materialIndex, const std::filesystem::path& sourceDirectory)
{
	ImportedMaterial importedMaterial;
	GltfMaterialFeatureReporting::ValidateFeatureSupport(material, materialIndex);
	GltfMaterialPropertyMapper::Apply(material, importedMaterial);
	GltfMaterialTextureMapper::Apply(material, materialIndex, sourceDirectory, importedMaterial);
	return importedMaterial;
}
