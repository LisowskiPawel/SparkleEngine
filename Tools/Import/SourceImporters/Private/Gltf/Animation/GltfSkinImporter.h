#pragma once

#include "SourceImportOutput.h"

#include <DirectXMath.h>

#include <cstddef>

struct cgltf_accessor;
struct cgltf_data;
struct cgltf_skin;

namespace GltfSkinImporter
{
	ImportedSkeletonIndex ImportSkeleton(const cgltf_data* data, const cgltf_skin* skin, SourceImportOutput& output);
	DirectX::XMMATRIX ComputeSkinReferenceToWorldTransform(const cgltf_skin* skin);
	ImportedSkinInfluence ReadSkinInfluence(const cgltf_accessor* joints0, const cgltf_accessor* weights0, const cgltf_accessor* joints1, const cgltf_accessor* weights1, std::size_t vertexIndex);
}
