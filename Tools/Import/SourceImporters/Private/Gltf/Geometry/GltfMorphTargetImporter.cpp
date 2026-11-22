#include "PCH.h"

#include "Gltf/Geometry/GltfMorphTargetImporter.h"

#include "Gltf/GltfAccessorReader.h"
#include "Gltf/GltfCoordinateConverter.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>

namespace GltfMorphTargetImporter
{
	static const cgltf_accessor* FindMorphAttribute(const cgltf_morph_target& target, cgltf_attribute_type type)
	{
		for (cgltf_size attributeIndex = 0; attributeIndex < target.attributes_count; ++attributeIndex)
		{
			if (target.attributes[attributeIndex].type == type)
			{
				const cgltf_accessor* accessor = target.attributes[attributeIndex].data;
				if (accessor->type != cgltf_type_vec3)
				{
					throw Diagnostics::Error("glTF morph-target attribute must contain one float3 delta per vertex.");
				}
				return accessor;
			}
		}

		return nullptr;
	}

	std::vector<ImportedMorphTarget> ImportMorphTargets(const cgltf_mesh& mesh, const cgltf_primitive& primitive, std::uint32_t vertexCount)
	{
		std::vector<ImportedMorphTarget> morphTargets;
		morphTargets.reserve(primitive.targets_count);

		for (cgltf_size targetIndex = 0; targetIndex < primitive.targets_count; ++targetIndex)
		{
			const cgltf_morph_target& sourceTarget = primitive.targets[targetIndex];
			const cgltf_accessor* positions = FindMorphAttribute(sourceTarget, cgltf_attribute_type_position);
			const cgltf_accessor* normals = FindMorphAttribute(sourceTarget, cgltf_attribute_type_normal);
			const cgltf_accessor* tangents = FindMorphAttribute(sourceTarget, cgltf_attribute_type_tangent);
			if (positions == nullptr && normals == nullptr && tangents == nullptr)
			{
				throw Diagnostics::Error("glTF morph target has no supported delta attributes.");
			}

			ImportedMorphTarget importedTarget;
			const bool bHasName = targetIndex < mesh.target_names_count && mesh.target_names[targetIndex] != nullptr;
			importedTarget.name = bHasName ? mesh.target_names[targetIndex] : "";
			importedTarget.defaultWeight = targetIndex < mesh.weights_count ? mesh.weights[targetIndex] : 0.0f;
			if (!std::isfinite(importedTarget.defaultWeight))
			{
				throw Diagnostics::Error("glTF morph target has a non-finite default weight.");
			}
			importedTarget.deltas.resize(vertexCount);
			for (std::uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				ImportedMorphTargetDelta& delta = importedTarget.deltas[vertexIndex];
				if (positions != nullptr)
				{
					const DirectX::XMFLOAT3 value = GltfAccessorReader::ReadFloat3(positions, vertexIndex);
					delta.position = GltfCoordinateConverter::ConvertMorphPositionDelta(value);
				}
				if (normals != nullptr)
				{
					const DirectX::XMFLOAT3 value = GltfAccessorReader::ReadFloat3(normals, vertexIndex);
					delta.normal = GltfCoordinateConverter::ConvertMorphNormalDelta(value);
				}
				if (tangents != nullptr)
				{
					const DirectX::XMFLOAT3 value = GltfAccessorReader::ReadFloat3(tangents, vertexIndex);
					delta.tangent = GltfCoordinateConverter::ConvertMorphTangentDelta(value);
				}
			}

			morphTargets.push_back(std::move(importedTarget));
		}

		return morphTargets;
	}

	std::vector<float> BuildNodeMorphWeights(const cgltf_node& node, std::size_t targetCount)
	{
		const cgltf_float* sourceWeights = node.weights_count > 0 ? node.weights : node.mesh->weights;
		std::vector<float> weights(targetCount, 0.0f);
		if (sourceWeights != nullptr)
		{
			std::copy_n(sourceWeights, targetCount, weights.begin());
		}
		const bool bWeightsFinite = std::ranges::all_of(weights, [](float weight) { return std::isfinite(weight); });
		if (!bWeightsFinite)
		{
			throw Diagnostics::Error("glTF node has a non-finite morph weight.");
		}

		return weights;
	}
}
