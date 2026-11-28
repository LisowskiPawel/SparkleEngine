#include "PCH.h"

#include "Gltf/Geometry/GltfGeometryImporter.h"

#include "Gltf/Geometry/GltfMeshGeometryExtractor.h"
#include "Gltf/Geometry/GltfMeshInstancingImporter.h"
#include "Gltf/Geometry/GltfMorphTargetImporter.h"
#include "Gltf/GltfCoordinateConverter.h"
#include "Gltf/Animation/GltfSkinImporter.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <cstdint>
#include <format>

struct GltfGeometryImporter::NodeImportContext final
{
	const cgltf_data& Data;
	const cgltf_node& Node;
	const GltfMeshGpuInstancingTransforms* MeshGpuInstancingTransforms = nullptr;
	DirectX::XMMATRIX WorldTransform;
	ImportedSkeletonIndex SkeletonIndex = kInvalidImportedSkeletonIndex;
	std::uint32_t NodeIndex = 0;
};

std::size_t GltfGeometryImporter::CountImportedMeshInstances(const cgltf_data& data)
{
	std::size_t totalPrimitives = 0;
	for (cgltf_size nodeIndex = 0; nodeIndex < data.nodes_count; ++nodeIndex)
	{
		const cgltf_node& node = data.nodes[nodeIndex];
		if (node.mesh)
		{
			std::size_t meshInstanceCount = 1;
			if (node.has_mesh_gpu_instancing && node.mesh_gpu_instancing.attributes_count > 0)
			{
				const cgltf_accessor* firstAccessor = node.mesh_gpu_instancing.attributes[0].data;
				if (firstAccessor == nullptr)
				{
					throw Diagnostics::Error("glTF mesh GPU instancing has no transform accessor.");
				}
				meshInstanceCount = firstAccessor->count;
			}

			totalPrimitives += node.mesh->primitives_count * meshInstanceCount;
		}
	}

	return totalPrimitives;
}

std::string GltfGeometryImporter::BuildPrimitiveLabel(const cgltf_node& node, std::size_t primitiveIndex)
{
	const std::string nodeName = node.name ? node.name : std::string("<unnamed-node>");
	return std::format("node '{}' primitive {}", nodeName, primitiveIndex);
}

void GltfGeometryImporter::ImportGeometry(const cgltf_data& data, ImportedScene& importedScene)
{
	for (cgltf_size nodeIndex = 0; nodeIndex < data.nodes_count; ++nodeIndex)
	{
		const cgltf_node& node = data.nodes[nodeIndex];
		ImportNode(data, node, static_cast<std::uint32_t>(nodeIndex), importedScene);
	}
}

void GltfGeometryImporter::ImportNode(const cgltf_data& data, const cgltf_node& node, std::uint32_t nodeIndex, ImportedScene& importedScene)
{
	if (node.mesh == nullptr)
	{
		return;
	}

	GltfMeshGpuInstancingTransforms meshGpuInstancingTransforms;
	const std::string nodeLabel = node.name != nullptr ? node.name : std::format("node {}", nodeIndex);
	const bool bHasMeshGpuInstancing = node.has_mesh_gpu_instancing && node.mesh_gpu_instancing.attributes_count > 0;
	if (bHasMeshGpuInstancing)
	{
		meshGpuInstancingTransforms = GltfMeshInstancingImporter::ReadMeshGpuInstancingTransforms(node, nodeLabel);
	}

	const ImportedSkeletonIndex skeletonIndex = node.skin != nullptr ? GltfSkinImporter::ImportSkeleton(&data, node.skin, importedScene) : kInvalidImportedSkeletonIndex;
	if (node.skin != nullptr && skeletonIndex == kInvalidImportedSkeletonIndex)
	{
		throw Diagnostics::Error(std::format("glTF node '{}' has an invalid skin.", nodeLabel));
	}

	const NodeImportContext context{
	    .Data = data,
	    .Node = node,
	    .MeshGpuInstancingTransforms = bHasMeshGpuInstancing ? &meshGpuInstancingTransforms : nullptr,
	    .WorldTransform = node.skin == nullptr ? GltfCoordinateConverter::ComputeNodeWorldTransform(&node) : GltfSkinImporter::ComputeSkinReferenceToWorldTransform(node.skin),
	    .SkeletonIndex = skeletonIndex,
	    .NodeIndex = nodeIndex};

	for (cgltf_size primitiveIndex = 0; primitiveIndex < node.mesh->primitives_count; ++primitiveIndex)
	{
		ImportPrimitive(context, node.mesh->primitives[primitiveIndex], static_cast<std::uint32_t>(primitiveIndex), importedScene);
	}
}

void GltfGeometryImporter::ImportPrimitive(const NodeImportContext& context, const cgltf_primitive& primitive, std::uint32_t primitiveIndex, ImportedScene& importedScene)
{
	const std::string primitiveLabel = BuildPrimitiveLabel(context.Node, primitiveIndex);
	const ImportedMeshPrimitiveIndex importedPrimitiveIndex = ResolveImportedPrimitive(context, primitive, primitiveIndex, primitiveLabel, importedScene);

	const ImportedMeshGeometry& geometry = importedScene.meshPrimitives[importedPrimitiveIndex].geometry;
	ValidateDeformation(geometry, context.SkeletonIndex, primitiveLabel, importedScene);

	const ImportedMaterialIndex materialIndex = ResolveMaterialIndex(context.Data, primitive, primitiveLabel, importedScene);
	const std::vector<float> morphWeights = geometry.HasMorphTargets() ? GltfMorphTargetImporter::BuildNodeMorphWeights(context.Node, geometry.deformation.morphTargets.size()) : std::vector<float>{};
	AppendInstances(context, importedPrimitiveIndex, materialIndex, morphWeights, importedScene);
}

ImportedMeshPrimitiveIndex GltfGeometryImporter::ResolveImportedPrimitive(
    const NodeImportContext& context,
    const cgltf_primitive& primitive,
    std::uint32_t primitiveIndex,
    std::string_view primitiveLabel,
    ImportedScene& importedScene)
{
	if (primitive.type != cgltf_primitive_type_triangles || primitive.has_draco_mesh_compression)
	{
		throw Diagnostics::Error(std::format("glTF {} {}.", primitiveLabel, primitive.has_draco_mesh_compression ? "uses unsupported Draco compression" : "is not a triangle primitive"));
	}

	const std::uint32_t sourceMeshIndex = static_cast<std::uint32_t>(cgltf_mesh_index(&context.Data, context.Node.mesh));
	{
		const ImportedMeshPrimitiveIndex existing = FindImportedPrimitiveIndex(importedScene, sourceMeshIndex, primitiveIndex);

		if (existing != kInvalidImportedMeshPrimitiveIndex)
		{
			return existing;
		}
	}

	ImportedMeshGeometry meshGeometry;
	try
	{
		meshGeometry = GltfMeshGeometryExtractor::ExtractMeshGeometry(*context.Node.mesh, primitive);
	}
	catch (const Diagnostics::Error& error)
	{
		throw Diagnostics::Error(std::format("glTF {} geometry import failed: {}", primitiveLabel, error.what()));
	}

	ImportedMeshPrimitive primitiveEntry;
	primitiveEntry.geometry = std::move(meshGeometry);
	primitiveEntry.sourceMeshIndex = sourceMeshIndex;
	primitiveEntry.sourcePrimitiveIndex = primitiveIndex;
	const ImportedMeshPrimitiveIndex importedPrimitiveIndex = static_cast<ImportedMeshPrimitiveIndex>(importedScene.meshPrimitives.size());
	importedScene.meshPrimitives.push_back(std::move(primitiveEntry));
	return importedPrimitiveIndex;
}

void GltfGeometryImporter::ValidateDeformation(const ImportedMeshGeometry& geometry, ImportedSkeletonIndex skeletonIndex, std::string_view primitiveLabel, const ImportedScene& scene)
{
	if (geometry.HasMorphTargets() && !geometry.HasSkinInfluences())
	{
		throw Diagnostics::Error(std::format("glTF {} has morph targets but no skin; morph-only deformation is unsupported.", primitiveLabel));
	}
	if (geometry.HasSkinInfluences())
	{
		if (skeletonIndex == kInvalidImportedSkeletonIndex || skeletonIndex >= scene.skeletons.size())
		{
			throw Diagnostics::Error(std::format("glTF {} has skin influences without a bound skeleton.", primitiveLabel));
		}
		ValidateSkinInfluences(geometry, scene.skeletons[skeletonIndex], primitiveLabel);
	}
}

void GltfGeometryImporter::AppendInstances(
    const NodeImportContext& context,
    ImportedMeshPrimitiveIndex primitiveIndex,
    ImportedMaterialIndex materialIndex,
    std::span<const float> morphWeights,
    ImportedScene& importedScene)
{
	ImportedMeshInstance instance;
	instance.primitiveIndex = primitiveIndex;
	instance.materialIndex = materialIndex;
	instance.skeletonIndex = context.SkeletonIndex;
	instance.sourceNodeIndex = context.NodeIndex;
	instance.sourceNodeName = context.Node.name != nullptr ? context.Node.name : "";
	instance.morphWeights.assign(morphWeights.begin(), morphWeights.end());
	if (context.MeshGpuInstancingTransforms == nullptr)
	{
		DirectX::XMStoreFloat4x4(&instance.worldTransform, context.WorldTransform);
		importedScene.meshInstances.push_back(std::move(instance));
		return;
	}

	const GltfMeshGpuInstancingTransforms& transforms = *context.MeshGpuInstancingTransforms;
	instance.groupIndex = static_cast<ImportedMeshInstanceGroupIndex>(importedScene.meshInstanceGroups.size());
	const ImportedMeshInstanceIndex firstInstanceIndex = static_cast<ImportedMeshInstanceIndex>(importedScene.meshInstances.size());
	for (std::size_t instanceIndex = 0; instanceIndex < transforms.instanceCount; ++instanceIndex)
	{
		const DirectX::XMMATRIX authoredTransform = GltfMeshInstancingImporter::BuildMeshGpuInstancingTransform(transforms, instanceIndex);
		const DirectX::XMMATRIX worldTransform = authoredTransform * context.WorldTransform;
		DirectX::XMStoreFloat4x4(&instance.worldTransform, worldTransform);
		importedScene.meshInstances.push_back(instance);
	}

	importedScene.meshInstanceGroups.push_back(
	    ImportedMeshInstanceGroup{
	        .primitiveIndex = primitiveIndex,
	        .materialIndex = materialIndex,
	        .firstInstanceIndex = firstInstanceIndex,
	        .instanceCount = static_cast<std::uint32_t>(transforms.instanceCount),
	        .groupKind = ImportedMeshInstanceGroupKind::AuthoredInstanceGroup});
}

ImportedMaterialIndex GltfGeometryImporter::ResolveMaterialIndex(const cgltf_data& data, const cgltf_primitive& primitive, std::string_view primitiveLabel, const ImportedScene& importedScene)
{
	if (primitive.material == nullptr || importedScene.materials.empty())
	{
		return kInvalidImportedMaterialIndex;
	}

	const std::uint32_t materialIndex = static_cast<std::uint32_t>(primitive.material - data.materials);
	if (materialIndex >= importedScene.materials.size())
	{
		throw Diagnostics::Error(std::format("glTF {} references unknown material index {}.", primitiveLabel, materialIndex));
	}

	return materialIndex;
}

void GltfGeometryImporter::ValidateSkinInfluences(const ImportedMeshGeometry& geometry, const ImportedSkeleton& skeleton, std::string_view primitiveLabel)
{
	for (const ImportedSkinInfluence& influence : geometry.deformation.skinInfluences)
	{
		for (std::size_t influenceIndex = 0; influenceIndex < 8u; ++influenceIndex)
		{
			if (influence.jointWeights[influenceIndex] > 0.0f && influence.jointIndices[influenceIndex] >= skeleton.joints.size())
			{
				throw Diagnostics::Error(std::format("glTF {} has skin influences outside its bound skeleton.", primitiveLabel));
			}
		}
	}
}

ImportedMeshPrimitiveIndex GltfGeometryImporter::FindImportedPrimitiveIndex(const ImportedScene& scene, std::uint32_t sourceMeshIndex, std::uint32_t sourcePrimitiveIndex) noexcept
{
	for (std::size_t primitiveIndex = 0; primitiveIndex < scene.meshPrimitives.size(); ++primitiveIndex)
	{
		const ImportedMeshPrimitive& primitive = scene.meshPrimitives[primitiveIndex];
		if (primitive.sourceMeshIndex == sourceMeshIndex && primitive.sourcePrimitiveIndex == sourcePrimitiveIndex)
		{
			return static_cast<ImportedMeshPrimitiveIndex>(primitiveIndex);
		}
	}

	return kInvalidImportedMeshPrimitiveIndex;
}
