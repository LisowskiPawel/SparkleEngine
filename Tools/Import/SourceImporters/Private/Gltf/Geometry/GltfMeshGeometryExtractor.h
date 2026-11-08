#pragma once

#include "Types/ImportedGeometry.h"

struct cgltf_primitive;
struct cgltf_mesh;

namespace GltfMeshGeometryExtractor
{
	ImportedMeshGeometry ExtractMeshGeometry(const cgltf_mesh& mesh, const cgltf_primitive& primitive);

}
