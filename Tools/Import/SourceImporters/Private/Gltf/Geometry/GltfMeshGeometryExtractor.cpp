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

class GltfGeometryRequirements final
{
public:
	static bool RequiresTextureCoordinates(const cgltf_material* material) noexcept
	{
		return material != nullptr
		    && ((material->has_pbr_metallic_roughness
		            && (material->pbr_metallic_roughness.base_color_texture.texture != nullptr || material->pbr_metallic_roughness.metallic_roughness_texture.texture != nullptr))
		        || material->normal_texture.texture != nullptr || material->occlusion_texture.texture != nullptr || material->emissive_texture.texture != nullptr);
	}

	static bool RequiresTangents(const cgltf_material* material) noexcept { return material != nullptr && material->normal_texture.texture != nullptr; }
};

struct GltfMeshGeometryExtractor::Attributes
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

GltfMeshGeometryExtractor::Attributes GltfMeshGeometryExtractor::CollectAttributes(const cgltf_primitive& primitive) noexcept
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

bool GltfMeshGeometryExtractor::HasValidTangentFrame(const ImportedMeshGeometry& geometry) noexcept
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

void GltfMeshGeometryExtractor::ValidateAttributes(const cgltf_primitive& primitive, const Attributes& attributes)
{
	const bool bRequiredAccessorsPresent = attributes.Positions != nullptr && attributes.Normals != nullptr;
	const bool bTextureCoordinatesPresentmaterial = bRequiredAccessorsPresent && (!GltfGeometryRequirements::RequiresTextureCoordinates(primitive.material));
	const bool bCoordinatesPresentGuardCoordinates = bRequiredAccessorsPresent && !bTextureCoordinatesPresentmaterial;
	const bool bCoordinatesPresentCoordinates = bCoordinatesPresentGuardCoordinates && (attributes.TextureCoordinates != nullptr);
	const bool bCoordinatesPresentCoordinates2 = bTextureCoordinatesPresentmaterial || bCoordinatesPresentCoordinates;
	const bool bTextureCoordinatesPresent = bRequiredAccessorsPresent && bCoordinatesPresentCoordinates2;
	const bool bVertexCountUsablePositions = bTextureCoordinatesPresent && attributes.Positions->count != 0;
	const bool bVertexCountUsable = bVertexCountUsablePositions && (attributes.Positions->count <= (std::numeric_limits<std::uint32_t>::max)());
	
	if (!bVertexCountUsable)
	{
		throw Diagnostics::Error("glTF primitive contains unsupported or malformed vertex attributes.");
	}

	for (cgltf_size attributeIndex = 0; attributeIndex < primitive.attributes_count; ++attributeIndex)
	{
		const cgltf_attribute& attribute = primitive.attributes[attributeIndex];

		const bool bSupportedtype = attribute.type == cgltf_attribute_type_position || attribute.type == cgltf_attribute_type_normal;
		const bool bSupportedtype2 = bSupportedtype || (attribute.type == cgltf_attribute_type_tangent);
		const bool bSupportedindex = bSupportedtype2 && (attribute.index == 0);
		const bool bSupportedtype3 = bSupportedindex || (attribute.type == cgltf_attribute_type_texcoord);
		const bool bSupportedtype4 = bSupportedtype3 || (attribute.type == cgltf_attribute_type_color);
		const bool bSupportedtype5 = attribute.type == cgltf_attribute_type_joints || attribute.type == cgltf_attribute_type_weights;
		const bool bSupportedindex2 = attribute.index == 0 || attribute.index == 1;
		const bool bSupportedindex3 = bSupportedtype5 && bSupportedindex2;
		const bool bSupported = bSupportedtype4 || bSupportedindex3;

		if (!bSupported || attribute.data == nullptr || attribute.data->count != attributes.Positions->count)
		{
			throw Diagnostics::Error("glTF primitive contains unsupported or malformed vertex attributes.");
		}
	}

	const bool bPositionNormalShapesValid = attributes.Positions->type == cgltf_type_vec3 && attributes.Normals->type == cgltf_type_vec3;
	const bool bShapeValidTextureCoordinates = bPositionNormalShapesValid && (attributes.TextureCoordinates == nullptr);
	const bool bShapeValidGuardTextureCoordinates = bPositionNormalShapesValid && !bShapeValidTextureCoordinates;
	const bool bShapeValidTextureCoordinates2 = bShapeValidGuardTextureCoordinates && (attributes.TextureCoordinates->type == cgltf_type_vec2);
	const bool bShapeValidTextureCoordinates3 = bShapeValidTextureCoordinates || bShapeValidTextureCoordinates2;
	const bool bTextureCoordinateShapeValid = bPositionNormalShapesValid && bShapeValidTextureCoordinates3;
	const bool bTangentShapeValidTangents = bTextureCoordinateShapeValid && (attributes.Tangents == nullptr);
	const bool bTangentShapeValidGuardTangents = bTextureCoordinateShapeValid && !bTangentShapeValidTangents;
	const bool bTangentShapeValidTangents2 = bTangentShapeValidGuardTangents && (attributes.Tangents->type == cgltf_type_vec4);
	const bool bTangentShapeValidTangents3 = bTangentShapeValidTangents || bTangentShapeValidTangents2;
	const bool bTangentShapeValid = bTextureCoordinateShapeValid && bTangentShapeValidTangents3;
	const bool bColorShapeValidColors = bTangentShapeValid && (attributes.Colors == nullptr);
	const bool bColorShapeValidGuardColors = bTangentShapeValid && !bColorShapeValidColors;
	const bool bColorShapeValidColors2 = bColorShapeValidGuardColors && (attributes.Colors->type == cgltf_type_vec3);
	const bool bColorShapeValidColors3 = bColorShapeValidColors || bColorShapeValidColors2;
	const bool bColorShapeValidGuardColors2 = bTangentShapeValid && !bColorShapeValidColors3;
	const bool bColorShapeValidColors4 = bColorShapeValidGuardColors2 && (attributes.Colors->type == cgltf_type_vec4);
	const bool bColorShapeValidColors5 = bColorShapeValidColors3 || bColorShapeValidColors4;
	const bool bColorShapeValid = bTangentShapeValid && bColorShapeValidColors5;
	const bool bPrimarySkinSetPaired = bColorShapeValid && (attributes.Joints0 == nullptr) == (attributes.Weights0 == nullptr);
	const bool bAttributeShapesValid = bPrimarySkinSetPaired && (attributes.Joints1 == nullptr) == (attributes.Weights1 == nullptr);
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

void GltfMeshGeometryExtractor::PopulateVertices(const Attributes& attributes, std::uint32_t vertexCount, ImportedMeshGeometry& geometry)
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

void GltfMeshGeometryExtractor::PopulateIndices(const cgltf_primitive& primitive, std::uint32_t vertexCount, ImportedMeshGeometry& geometry)
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

ImportedMeshGeometry GltfMeshGeometryExtractor::ExtractMeshGeometry(const cgltf_mesh& mesh, const cgltf_primitive& primitive)
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
	if (GltfGeometryRequirements::RequiresTangents(primitive.material))
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
