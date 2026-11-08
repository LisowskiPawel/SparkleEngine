#pragma once

#include "RHI/Public/RayTracing/RhiRayTracingDesc.h"

#include <DirectXMath.h>

#include <cstddef>
#include <cstdint>
#include <vector>

class GpuMesh;
struct MeshDraw;
struct PreparedRenderScene;
struct VertexSkinInfluence;

bool AreRayTracingBlasGeometriesEqual(const RhiRayTracingGeometryDesc& left, const RhiRayTracingGeometryDesc& right) noexcept;
std::uint64_t AlignRayTracingBlasBufferSize(std::uint64_t sizeInBytes, std::uint64_t alignment) noexcept;
bool IsSkinnedRayTracingDraw(const MeshDraw& draw) noexcept;
void ComputeRayTracingSkinnedPositions(const PreparedRenderScene& preparedScene, const MeshDraw& draw, const GpuMesh& mesh, std::vector<DirectX::XMFLOAT3>& outPositions) noexcept;
