#pragma once

#include "Types/ImportedMorphTarget.h"

#include <cstddef>
#include <cstdint>
#include <vector>

struct cgltf_mesh;
struct cgltf_node;
struct cgltf_primitive;

namespace GltfMorphTargetImporter
{
	std::vector<ImportedMorphTarget> ImportMorphTargets(const cgltf_mesh& mesh, const cgltf_primitive& primitive, std::uint32_t vertexCount);
	std::vector<float> BuildNodeMorphWeights(const cgltf_node& node, std::size_t targetCount);
}
