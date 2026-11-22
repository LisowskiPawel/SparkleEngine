#pragma once

#include "Types/ImportedScene.h"

#include <DirectXMath.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

struct cgltf_data;
struct cgltf_node;
struct cgltf_primitive;

struct GltfMeshGpuInstancingTransforms;

class GltfGeometryImporter final
{
public:
	static std::size_t CountImportedMeshInstances(const cgltf_data& data);
	static void ImportGeometry(const cgltf_data& data, ImportedScene& importedScene);

private:
	struct NodeImportContext;

	static void ImportNode(const cgltf_data& data, const cgltf_node& node, std::uint32_t nodeIndex, ImportedScene& importedScene);
	static void ImportPrimitive(const NodeImportContext& context, const cgltf_primitive& primitive, std::uint32_t primitiveIndex, ImportedScene& importedScene);
	static ImportedMeshPrimitiveIndex ResolveImportedPrimitive(
	    const NodeImportContext& context,
	    const cgltf_primitive& primitive,
	    std::uint32_t primitiveIndex,
	    std::string_view primitiveLabel,
	    ImportedScene& importedScene);
	static void ValidateDeformation(const ImportedMeshGeometry& geometry, ImportedSkeletonIndex skeletonIndex, std::string_view primitiveLabel, const ImportedScene& scene);
	static void AppendInstances(
	    const NodeImportContext& context,
	    ImportedMeshPrimitiveIndex primitiveIndex,
	    ImportedMaterialIndex materialIndex,
	    std::span<const float> morphWeights,
	    ImportedScene& importedScene);
	static ImportedMaterialIndex ResolveMaterialIndex(const cgltf_data& data, const cgltf_primitive& primitive, std::string_view primitiveLabel, const ImportedScene& importedScene);
	static void ValidateSkinInfluences(const ImportedMeshGeometry& geometry, const ImportedSkeleton& skeleton, std::string_view primitiveLabel);
	static ImportedMeshPrimitiveIndex FindImportedPrimitiveIndex(const ImportedScene& scene, std::uint32_t sourceMeshIndex, std::uint32_t sourcePrimitiveIndex) noexcept;
	static std::string BuildPrimitiveLabel(const cgltf_node& node, std::size_t primitiveIndex);
};
