#pragma once

#include "GameFramework/Public/Rendering/RenderAssetHandles.h"
#include "RayTracing/RayTracingHitData.h"
#include "Scene/Meshes/MeshSkinningData.h"
#include "ShaderData/MeshInstanceShaderData.h"
#include "ShaderData/MorphTargetShaderData.h"

#include <DirectXMath.h>

#include <cstdint>
#include <vector>

struct MeshMorphTargetDelta;

struct GpuMeshPreparedData final
{
	ImmutableRenderMeshHandle Source;
	DirectX::XMFLOAT3 LocalBoundsMin = {};
	DirectX::XMFLOAT3 LocalBoundsMax = {};
	std::vector<RayTracingHitVertex> RayTracingVertices;
	std::vector<std::uint32_t> RayTracingIndices;
	std::vector<VertexSkinInfluence> SkinInfluences;
	std::vector<VertexSkinInfluenceData> GpuSkinInfluences;
	std::vector<MorphTargetDeltaData> MorphTargetDeltas;
	std::uint32_t MorphTargetCount = 0u;
	bool bHasLocalBounds = false;

	std::uint64_t GetDecodedByteSize() const noexcept;
	std::uint64_t GetResidentByteSize() const noexcept;
};

std::uint64_t CalculateGpuMeshDecodedByteSize(const ImmutableRenderMeshHandle& source);
GpuMeshPreparedData PrepareGpuMesh(const ImmutableRenderMeshHandle& source);
VertexSkinInfluenceData ConvertGpuMeshSkinInfluence(const VertexSkinInfluence& influence) noexcept;
