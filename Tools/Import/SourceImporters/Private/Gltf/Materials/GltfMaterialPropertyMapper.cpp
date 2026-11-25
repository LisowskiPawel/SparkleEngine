#include "PCH.h"

#include "Gltf/Materials/GltfMaterialPropertyMapper.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Types/ImportedMaterial.h"

#include <DirectXMath.h>
#include <cgltf.h>

#include <format>
#include <string>
#include <string_view>

namespace GltfMaterialPropertyMapper
{
	static void ValidateFeatureSupport(const cgltf_material& material, ImportedMaterialIndex materialIndex)
	{
		const std::string_view featureNames[] = {
		    material.has_pbr_specular_glossiness ? "KHR_materials_pbrSpecularGlossiness" : "",
		    material.unlit ? "KHR_materials_unlit" : "",
		    material.has_specular ? "KHR_materials_specular" : "",
		    material.has_iridescence ? "KHR_materials_iridescence" : "",
		    material.has_dispersion ? "KHR_materials_dispersion" : ""};

		std::string unsupportedFeatures;
		for (const std::string_view name : featureNames)
		{
			if (name.empty())
			{
				continue;
			}
			if (!unsupportedFeatures.empty())
			{
				unsupportedFeatures += ", ";
			}
			unsupportedFeatures += name;
		}
		if (!unsupportedFeatures.empty())
		{
			throw Diagnostics::Error(std::format("glTF material {} uses unsupported features [{}].", materialIndex, unsupportedFeatures));
		}
	}

	void Apply(const cgltf_material& material, ImportedMaterialIndex materialIndex, ImportedMaterial& importedMaterial)
	{
		ValidateFeatureSupport(material, materialIndex);
		importedMaterial.emissiveColor = DirectX::XMFLOAT3(material.emissive_factor[0], material.emissive_factor[1], material.emissive_factor[2]);
		if (material.has_emissive_strength)
		{
			importedMaterial.emissiveColor.x *= material.emissive_strength.emissive_strength;
			importedMaterial.emissiveColor.y *= material.emissive_strength.emissive_strength;
			importedMaterial.emissiveColor.z *= material.emissive_strength.emissive_strength;
		}

		importedMaterial.alphaCutoff = material.alpha_cutoff;
		importedMaterial.bDoubleSided = material.double_sided;

		switch (material.alpha_mode)
		{
			case cgltf_alpha_mode_mask:
				importedMaterial.alphaMode = ImportedAlphaMode::Mask;
				break;

			case cgltf_alpha_mode_blend:
				importedMaterial.alphaMode = ImportedAlphaMode::Blend;
				break;

			case cgltf_alpha_mode_opaque:
			default:
				importedMaterial.alphaMode = ImportedAlphaMode::Opaque;
				break;
		}

		if (material.has_pbr_metallic_roughness)
		{
			const cgltf_pbr_metallic_roughness& pbr = material.pbr_metallic_roughness;
			importedMaterial.baseColor = DirectX::XMFLOAT4(pbr.base_color_factor[0], pbr.base_color_factor[1], pbr.base_color_factor[2], pbr.base_color_factor[3]);
			importedMaterial.metallic = pbr.metallic_factor;
			importedMaterial.roughness = pbr.roughness_factor;
		}

		if (material.has_ior)
		{
			const float f0 = (material.ior.ior - 1.0f) / (material.ior.ior + 1.0f);
			importedMaterial.f0 = f0 * f0;
		}
	}
}
