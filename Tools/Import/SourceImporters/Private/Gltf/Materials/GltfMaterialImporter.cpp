#include "PCH.h"

#include "Gltf/Materials/GltfMaterialImporter.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Gltf/Materials/GltfMaterialTextureMapper.h"
#include "Gltf/Materials/GltfMaterialPropertyMapper.h"
#include "Types/ImportedScene.h"

#include <cgltf.h>

#include <algorithm>
#include <format>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

namespace GltfMaterialImporter
{
	static void ValidateRequiredExtensions(const cgltf_data& data)
	{
		static constexpr std::string_view unsupportedExtensions[] =
		    {"KHR_materials_anisotropy", "KHR_materials_clearcoat", "KHR_materials_sheen", "KHR_materials_transmission", "KHR_materials_volume"};

		for (cgltf_size index = 0; index < data.extensions_required_count; ++index)
		{
			const std::string_view extension = data.extensions_required[index];
			const bool bUnsupported = std::ranges::find(unsupportedExtensions, extension) != std::end(unsupportedExtensions);
			if (bUnsupported)
			{
				throw Diagnostics::Error(std::format("Required glTF material extension '{}' has no shading implementation.", extension));
			}
		}
	}

	void ImportMaterials(const cgltf_data& data, const std::filesystem::path& sourceDirectory, ImportedScene& importedScene)
	{
		ValidateRequiredExtensions(data);
		for (cgltf_size materialIndex = 0; materialIndex < data.materials_count; ++materialIndex)
		{
			const cgltf_material& sourceMaterial = data.materials[materialIndex];
			const auto importedIndex = static_cast<ImportedMaterialIndex>(materialIndex);
			ImportedMaterial material;
			GltfMaterialPropertyMapper::Apply(sourceMaterial, importedIndex, material);
			GltfMaterialTextureMapper::Apply(sourceMaterial, importedIndex, sourceDirectory, material);
			importedScene.materials.push_back(std::move(material));
		}
	}
}
