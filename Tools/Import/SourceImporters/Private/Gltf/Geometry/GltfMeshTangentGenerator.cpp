#include "PCH.h"

#include "Gltf/Geometry/GltfMeshTangentGenerator.h"

#include "Gltf/Geometry/GltfTangentFrameSetGenerator.h"
#include "Gltf/Geometry/GltfTangentVertexRemapper.h"

void GltfMeshTangentGenerator::GenerateTangents(ImportedMeshGeometry& geometry)
{
	GltfGeneratedTangentFrameSet frames = GltfTangentFrameSetGenerator::Generate(geometry);
	GltfTangentVertexRemapper::Apply(geometry, frames);
}
