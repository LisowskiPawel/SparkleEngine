#include "PCH.h"

#include "Gltf/GltfMaterialTextureMapper.h"

#include "Core/Public/Diagnostics/Error.h"
#include "SourceTexturePathResolver.h"
#include "Types/ImportedMaterial.h"

#include <cgltf.h>

#include <cstdint>
#include <format>
#include <string_view>

namespace GltfMaterialTextureMapper
{
	static std::filesystem::path ResolveTexturePath(const cgltf_texture_view& textureView, ImportedMaterialIndex materialIndex, const std::filesystem::path& sourceDirectory, std::string_view slotName)
	{
		const std::uint32_t texCoord = textureView.has_transform && textureView.transform.has_texcoord ? static_cast<std::uint32_t>(textureView.transform.texcoord)
		                                                                                               : static_cast<std::uint32_t>(textureView.texcoord);

		if (texCoord != 0u)
		{
			throw Diagnostics::Error(std::format("glTF material {} uses an unsupported {} texture coordinate mapping.", materialIndex, slotName));
		}
		const cgltf_texture& texture = *textureView.texture;
		if (texture.has_basisu || texture.has_webp)
		{
			throw Diagnostics::Error(std::format("glTF material {} uses an unsupported {} texture encoding.", materialIndex, slotName));
		}

		if (texture.image && texture.image->uri)
		{
			const std::string_view texturePathString = texture.image->uri;
			if (texturePathString.empty())
			{
				throw Diagnostics::Error(std::format("glTF material {} has an empty {} texture URI.", materialIndex, slotName));
			}

			return SourceTexturePathResolver::ResolveExistingFile(sourceDirectory, texturePathString);
		}

		if (texture.image && texture.image->buffer_view)
		{
			throw Diagnostics::Error(std::format("glTF material {} uses an unsupported embedded {} texture.", materialIndex, slotName));
		}

		throw Diagnostics::Error(std::format("glTF material {} has no supported {} texture source.", materialIndex, slotName));
	}

	static TextureCoordinateMapping BuildTextureMapping(const cgltf_texture_view& textureView)
	{
		TextureCoordinateMapping mapping;
		mapping.Strength = textureView.scale;
		if (textureView.has_transform)
		{
			mapping.Offset = {textureView.transform.offset[0], textureView.transform.offset[1]};
			mapping.Scale = {textureView.transform.scale[0], textureView.transform.scale[1]};
			mapping.Rotation = textureView.transform.rotation;
			mapping.TexCoord = textureView.transform.has_texcoord ? static_cast<std::uint32_t>(textureView.transform.texcoord) : static_cast<std::uint32_t>(textureView.texcoord);
		}
		else
		{
			mapping.TexCoord = static_cast<std::uint32_t>(textureView.texcoord);
		}

		if (textureView.texture && textureView.texture->sampler)
		{
			auto addressMode = [](cgltf_wrap_mode wrap)
			{
				switch (wrap)
				{
					case cgltf_wrap_mode_clamp_to_edge:
						return TextureAddressMode::ClampToEdge;
					case cgltf_wrap_mode_mirrored_repeat:
						return TextureAddressMode::MirroredRepeat;
					case cgltf_wrap_mode_repeat:
					default:
						return TextureAddressMode::Repeat;
				}
			};

			mapping.AddressU = addressMode(textureView.texture->sampler->wrap_s);
			mapping.AddressV = addressMode(textureView.texture->sampler->wrap_t);
		}
		return mapping;
	}

	static void AssignTexture(
	    const cgltf_texture_view& textureView,
	    ImportedMaterialIndex materialIndex,
	    const std::filesystem::path& sourceDirectory,
	    TextureGroup textureGroup,
	    std::string_view slotName,
	    ImportedMaterial& importedMaterial,
	    TextureChannelMask channelMask = TextureChannelMask::Rgba)
	{
		if (!textureView.texture)
		{
			return;
		}

		const std::filesystem::path texturePath = ResolveTexturePath(textureView, materialIndex, sourceDirectory, slotName);
		importedMaterial.textureSources.push_back({textureGroup, texturePath, channelMask, BuildTextureMapping(textureView)});
	}

	static void AssignPackedMetallicRoughness(const cgltf_material& material, ImportedMaterialIndex materialIndex, const std::filesystem::path& sourceDirectory, ImportedMaterial& importedMaterial)
	{
		if (!material.has_pbr_metallic_roughness || !material.pbr_metallic_roughness.metallic_roughness_texture.texture)
		{
			return;
		}

		const cgltf_texture_view& textureView = material.pbr_metallic_roughness.metallic_roughness_texture;
		const std::filesystem::path texturePath = ResolveTexturePath(textureView, materialIndex, sourceDirectory, "metallic-roughness");
		const TextureCoordinateMapping mapping = BuildTextureMapping(textureView);
		importedMaterial.textureSources.push_back({TextureGroup::Roughness, texturePath, TextureChannelMask::Green, mapping});
		importedMaterial.textureSources.push_back({TextureGroup::Metallic, texturePath, TextureChannelMask::Blue, mapping});
	}

	void Apply(const cgltf_material& material, ImportedMaterialIndex materialIndex, const std::filesystem::path& sourceDirectory, ImportedMaterial& importedMaterial)
	{
		if (material.has_pbr_metallic_roughness)
		{
			AssignTexture(material.pbr_metallic_roughness.base_color_texture, materialIndex, sourceDirectory, TextureGroup::Diffuse, "base-color", importedMaterial);
		}
		AssignTexture(material.normal_texture, materialIndex, sourceDirectory, TextureGroup::NormalMap, "normal", importedMaterial);
		AssignTexture(material.occlusion_texture, materialIndex, sourceDirectory, TextureGroup::AmbientOcclusion, "occlusion", importedMaterial, TextureChannelMask::Red);
		AssignTexture(material.emissive_texture, materialIndex, sourceDirectory, TextureGroup::Emissive, "emissive", importedMaterial);
		AssignPackedMetallicRoughness(material, materialIndex, sourceDirectory, importedMaterial);
	}
}
