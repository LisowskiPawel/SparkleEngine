#include "PCH.h"

#include "Gltf/Geometry/GltfMeshGeometryExtractor.h"

#include "Gltf/GltfAccessorReader.h"
#include "Gltf/Geometry/GltfMeshTangentGenerator.h"
#include "Gltf/Geometry/GltfMorphTargetImporter.h"
#include "Gltf/GltfCoordinateConverter.h"
#include "Gltf/Animation/GltfSkinImporter.h"
#include "Gltf/Geometry/GltfTangentFrameValidator.h"
#include "Gltf/Geometry/GltfVertexFrameBuilder.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <cstdint>
#include <format>
#include <limits>

namespace GltfMeshGeometryExtractor
{
	static bool RequiresTextureCoordinates(const cgltf_material* material) noexcept
	{
		return material != nullptr
		    && ((material->has_pbr_metallic_roughness
		            && (material->pbr_metallic_roughness.base_color_texture.texture != nullptr || material->pbr_metallic_roughness.metallic_roughness_texture.texture != nullptr))
		        || material->normal_texture.texture != nullptr || material->occlusion_texture.texture != nullptr || material->emissive_texture.texture != nullptr);
	}

	static bool RequiresTangents(const cgltf_material* material) noexcept
	{
		return material != nullptr && material->normal_texture.texture != nullptr;
	}

	struct Attributes
	{
		const cgltf_accessor* Positions = nullptr;
		const cgltf_accessor* Normals = nullptr;
		const cgltf_accessor* TextureCoordinates = nullptr;
		const cgltf_accessor* Tangents = nullptr;
		const cgltf_accessor* Colors = nullptr;
		const cgltf_accessor* Joints0 = nullptr;
		const cgltf_accessor* Weights0 = nullptr;
		const cgltf_accessor* Joints1 = nullptr;
		const cgltf_accessor* Weights1 = nullptr;

		bool HasSkinInfluences() const noexcept { return Joints0 != nullptr && Weights0 != nullptr; }
	};

	static Attributes CollectAttributes(const cgltf_primitive& primitive) noexcept
	{
		return Attributes{
		    .Positions = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_position),
		    .Normals = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_normal),
		    .TextureCoordinates = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_texcoord),
		    .Tangents = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_tangent),
		    .Colors = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_color),
		    .Joints0 = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_joints, 0),
		    .Weights0 = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_weights, 0),
		    .Joints1 = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_joints, 1),
		    .Weights1 = GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_weights, 1)};
	}

	static bool HasValidTangentFrame(const ImportedMeshGeometry& geometry) noexcept
	{
		try
		{
			GltfTangentFrameValidator::Validate(geometry);
			return true;
		}
		catch (const Diagnostics::Error&)
		{
			return false;
		}
	}

	static void ValidateAttributes(const cgltf_primitive& primitive, const Attributes& attributes)
	{
		const bool bRequiredAccessors = attributes.Positions != nullptr && attributes.Normals != nullptr;
		const bool bNeedsCoordinates = bRequiredAccessors && RequiresTextureCoordinates(primitive.material);
		const bool bCoordinatesReady = !bNeedsCoordinates || attributes.TextureCoordinates != nullptr;
		const bool bAccessorsReady = bRequiredAccessors && bCoordinatesReady;
		const bool bHasVertices = bAccessorsReady && attributes.Positions->count != 0;
		const bool bVertexCountValid = bHasVertices && attributes.Positions->count <= (std::numeric_limits<std::uint32_t>::max)();

		if (!bVertexCountValid)
		{
			throw Diagnostics::Error("glTF primitive contains unsupported or malformed vertex attributes.");
		}

		for (cgltf_size attributeIndex = 0; attributeIndex < primitive.attributes_count; ++attributeIndex)
		{
			const cgltf_attribute& attribute = primitive.attributes[attributeIndex];

			const bool bSupportedType = attribute.type == cgltf_attribute_type_position || attribute.type == cgltf_attribute_type_normal;
			const bool bGeometryAttribute = bSupportedType || (attribute.type == cgltf_attribute_type_tangent);
			const bool bSupportedIndex = bGeometryAttribute && (attribute.index == 0);
			const bool bCoordinateAttribute = bSupportedIndex || (attribute.type == cgltf_attribute_type_texcoord);
			const bool bColorAttribute = bCoordinateAttribute || (attribute.type == cgltf_attribute_type_color);
			const bool bSkinAttribute = attribute.type == cgltf_attribute_type_joints || attribute.type == cgltf_attribute_type_weights;
			const bool bSupportedSkinSet = attribute.index == 0 || attribute.index == 1;
			const bool bSkinSetAccepted = bSkinAttribute && bSupportedSkinSet;
			const bool bSupported = bColorAttribute || bSkinSetAccepted;

			const bool bSupportedAccessor = bSupported && attribute.data != nullptr;
			const bool bCountMatches = bSupportedAccessor && attribute.data->count == attributes.Positions->count;
			if (!bCountMatches)
			{
				throw Diagnostics::Error("glTF primitive contains unsupported or malformed vertex attributes.");
			}
		}

		const bool bVectorsValid = attributes.Positions->type == cgltf_type_vec3 && attributes.Normals->type == cgltf_type_vec3;
		const bool bCoordinatesValid = attributes.TextureCoordinates == nullptr || attributes.TextureCoordinates->type == cgltf_type_vec2;
		const bool bTangentsValid = attributes.Tangents == nullptr || attributes.Tangents->type == cgltf_type_vec4;
		const bool bRgbOrNoColors = attributes.Colors == nullptr || attributes.Colors->type == cgltf_type_vec3;
		const bool bRgbaColors = !bRgbOrNoColors && attributes.Colors->type == cgltf_type_vec4;
		const bool bColorsValid = bRgbOrNoColors || bRgbaColors;
		const bool bVertexShapesValid = bVectorsValid && bCoordinatesValid;
		const bool bExtraShapesValid = bTangentsValid && bColorsValid;
		const bool bShapesValid = bVertexShapesValid && bExtraShapesValid;
		const bool bPrimaryPaired = (attributes.Joints0 == nullptr) == (attributes.Weights0 == nullptr);
		const bool bSecondaryPaired = (attributes.Joints1 == nullptr) == (attributes.Weights1 == nullptr);
		const bool bSkinSetsPaired = bPrimaryPaired && bSecondaryPaired;
		const bool bAttributeShapesValid = bShapesValid && bSkinSetsPaired;
		const bool bJointSetsSupported = bAttributeShapesValid && GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_joints, 2) == nullptr;
		const bool bWeightSetsSupported = bJointSetsSupported && GltfAccessorReader::FindAttribute(primitive, cgltf_attribute_type_weights, 2) == nullptr;

		if (!bWeightSetsSupported)
		{
			throw Diagnostics::Error("glTF primitive contains unsupported or malformed vertex attributes.");
		}

		for (cgltf_size targetIndex = 0; targetIndex < primitive.targets_count; ++targetIndex)
		{
			const cgltf_morph_target& target = primitive.targets[targetIndex];
			for (cgltf_size attributeIndex = 0; attributeIndex < target.attributes_count; ++attributeIndex)
			{
				if (target.attributes[attributeIndex].type == cgltf_attribute_type_tangent && attributes.Tangents == nullptr)
				{
					throw Diagnostics::Error("A glTF morph target cannot provide tangent deltas when the base primitive omits tangents.");
				}
			}
		}
	}

	static void PopulateVertices(const Attributes& attributes, std::uint32_t vertexCount, ImportedMeshGeometry& geometry)
	{
		for (std::uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
		{
			ImportedVertex& vertex = geometry.vertices[vertexIndex];
			const DirectX::XMFLOAT3 sourcePosition = GltfAccessorReader::ReadFloat3(attributes.Positions, vertexIndex);
			vertex.position = GltfCoordinateConverter::ConvertPosition(sourcePosition);

			try
			{
				const DirectX::XMFLOAT3 sourceNormal = GltfAccessorReader::ReadFloat3(attributes.Normals, vertexIndex);
				vertex.normal = GltfVertexFrameBuilder::BuildNormal(GltfCoordinateConverter::ConvertNormal(sourceNormal));

				if (attributes.Tangents != nullptr)
				{
					const DirectX::XMFLOAT4 sourceTangent = GltfAccessorReader::ReadFloat4(attributes.Tangents, vertexIndex);
					vertex.tangent = GltfVertexFrameBuilder::BuildAuthoredTangent(GltfCoordinateConverter::ConvertTangentFrame(sourceTangent), vertex.normal);
				}
			}
			catch (const Diagnostics::Error& error)
			{
				throw Diagnostics::Error(std::format("glTF vertex {} frame validation failed: {}", vertexIndex, error.what()));
			}

			if (attributes.TextureCoordinates != nullptr)
			{
				vertex.uv = GltfAccessorReader::ReadFloat2(attributes.TextureCoordinates, vertexIndex);
			}

			if (attributes.Colors != nullptr)
			{
				if (attributes.Colors->type == cgltf_type_vec4)
				{
					vertex.color = GltfAccessorReader::ReadFloat4(attributes.Colors, vertexIndex);
				}
				else
				{
					const DirectX::XMFLOAT3 sourceColor = GltfAccessorReader::ReadFloat3(attributes.Colors, vertexIndex);
					vertex.color = {sourceColor.x, sourceColor.y, sourceColor.z, 1.0f};
				}
			}

			if (attributes.HasSkinInfluences())
			{
				geometry.deformation.skinInfluences[vertexIndex] = GltfSkinImporter::ReadSkinInfluence(attributes.Joints0, attributes.Weights0, attributes.Joints1, attributes.Weights1, vertexIndex);
			}
		}
	}

	static void PopulateIndices(const cgltf_primitive& primitive, std::uint32_t vertexCount, ImportedMeshGeometry& geometry)
	{
		geometry.indices = GltfAccessorReader::ReadIndices(primitive.indices);
		if (geometry.indices.size() < 3u || geometry.indices.size() % 3u != 0u)
		{
			throw Diagnostics::Error("glTF primitive contains incomplete triangle-index data.");
		}
		for (const std::uint32_t index : geometry.indices)
		{
			if (index >= vertexCount)
			{
				throw Diagnostics::Error("glTF primitive contains an out-of-range triangle index.");
			}
		}
		GltfCoordinateConverter::ConvertTriangleWinding(geometry.indices);
	}

	ImportedMeshGeometry ExtractMeshGeometry(const cgltf_mesh& mesh, const cgltf_primitive& primitive)
	{
		const Attributes attributes = CollectAttributes(primitive);
		ValidateAttributes(primitive, attributes);

		const std::uint32_t vertexCount = static_cast<std::uint32_t>(attributes.Positions->count);
		ImportedMeshGeometry geometry;
		geometry.Reserve(vertexCount, primitive.indices ? static_cast<std::uint32_t>(primitive.indices->count) : 0u);
		geometry.vertices.resize(vertexCount);
		if (attributes.HasSkinInfluences())
		{
			geometry.deformation.skinInfluences.resize(vertexCount);
		}

		bool bAuthoredTangentsImported = attributes.Tangents != nullptr;
		try
		{
			PopulateVertices(attributes, vertexCount, geometry);
		}
		catch (const Diagnostics::Error&)
		{
			if (!bAuthoredTangentsImported)
			{
				throw;
			}

			Attributes generatedTangentAttributes = attributes;
			generatedTangentAttributes.Tangents = nullptr;
			geometry.vertices.assign(vertexCount, ImportedVertex{});
			PopulateVertices(generatedTangentAttributes, vertexCount, geometry);
			bAuthoredTangentsImported = false;
		}
		PopulateIndices(primitive, vertexCount, geometry);
		geometry.deformation.morphTargets = GltfMorphTargetImporter::ImportMorphTargets(mesh, primitive, vertexCount);
		if (RequiresTangents(primitive.material))
		{
			if (bAuthoredTangentsImported && HasValidTangentFrame(geometry))
			{
				return geometry;
			}

			GltfMeshTangentGenerator::GenerateTangents(geometry);
			GltfTangentFrameValidator::Validate(geometry);
		}
		return geometry;
	}
}
