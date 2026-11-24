#pragma once

#include "Meshes/GpuMeshBuffer.h"
#include "Meshes/GpuMeshHandle.h"
#include "RayTracing/RayTracingHitData.h"
#include "RHI/Public/Descriptors/RhiDescriptorHandles.h"
#include "RHI/Public/RayTracing/RhiAccelerationStructureDesc.h"
#include "RHI/Public/Pipeline/RhiPipelineDesc.h"
#include "RHI/Public/Resources/RhiResourceHandles.h"
#include "Scene/Meshes/MeshSkinningData.h"
#include "ShaderData/MorphTargetShaderData.h"

#include <DirectXMath.h>

#include <cstdint>
#include <span>
#include <vector>

class RenderCommandContext;
class RenderCommandList;
class RenderHardwareInterface;
struct GpuMeshPreparedData;
struct MeshData;

struct GpuMeshBounds final
{
	DirectX::XMFLOAT3 Min = {};
	DirectX::XMFLOAT3 Max = {};
	bool bValid = false;
};

class GpuMesh final
{
public:
	explicit GpuMesh(GpuMeshHandle handle = {}) noexcept;
	~GpuMesh() noexcept;

	GpuMesh(const GpuMesh&) = delete;
	GpuMesh& operator=(const GpuMesh&) = delete;
	GpuMesh(GpuMesh&&) = delete;
	GpuMesh& operator=(GpuMesh&&) = delete;

	void Upload(RenderHardwareInterface& renderHardwareInterface, RenderCommandList& commandList, GpuMeshPreparedData preparedData);

	void Bind(RenderCommandContext& commandContext) const noexcept;

	std::uint32_t GetIndexCount() const noexcept { return m_indexCount; }

	std::uint32_t GetVertexCount() const noexcept { return m_vertexCount; }

	bool IsValid() const noexcept { return m_vertexBuffer.GetResource() && m_indexBuffer.GetResource(); }

	GpuMeshHandle GetHandle() const noexcept { return m_handle; }

	RhiVertexBufferView GetVertexBufferView() const noexcept;
	RhiIndexBufferView GetIndexBufferView() const noexcept;
	const RhiVertexInputDeclaration& GetVertexInputDeclaration() const noexcept;

	RhiPrimitiveTopology GetPrimitiveTopology() const noexcept { return RhiPrimitiveTopology::TriangleList; }

	ERhiFrontFaceWinding GetFrontFaceWinding() const noexcept { return ERhiFrontFaceWinding::Clockwise; }

	bool UsesDepthClipping() const noexcept { return true; }

	RhiOwnedResourceHandle GetVertexBufferResource() const noexcept { return m_vertexBuffer.GetResource(); }

	RhiOwnedResourceHandle GetIndexBufferResource() const noexcept { return m_indexBuffer.GetResource(); }

	RhiGpuDescriptorHandle GetSkinInfluencesShaderResourceView() const noexcept { return m_skinInfluences.GetShaderResourceView(); }

	RhiGpuDescriptorHandle GetMorphTargetDeltasShaderResourceView() const noexcept { return m_morphTargets.GetShaderResourceView(); }

	RhiRayTracingGeometryDesc GetRayTracingGeometry() const noexcept;

	const GpuMeshBounds& GetLocalBounds() const noexcept { return m_localBounds; }

	bool HasRayTracingHitData() const noexcept { return !m_rayTracingHitVertices.empty() && !m_rayTracingHitIndices.empty(); }

	std::span<const RayTracingHitVertex> GetRayTracingHitVertices() const noexcept { return m_rayTracingHitVertices; }

	std::span<const std::uint32_t> GetRayTracingHitIndices() const noexcept { return m_rayTracingHitIndices; }

	bool HasSkinInfluences() const noexcept { return m_cpuSkinInfluences.size() == m_vertexCount; }

	std::span<const VertexSkinInfluence> GetSkinInfluences() const noexcept { return m_cpuSkinInfluences; }

	std::uint32_t GetMorphTargetCount() const noexcept { return m_morphTargetCount; }

	std::span<const MorphTargetDeltaData> GetMorphTargetDeltas() const noexcept { return m_morphTargetDeltas; }

private:
	void CreateGeometryBuffers(RenderCommandList& commandList, const MeshData& meshData);
	void CreateVertexBuffer(RenderCommandList& commandList, const MeshData& meshData);
	void CreateIndexBuffer(RenderCommandList& commandList, const MeshData& meshData);
	void CreateDeformationBuffers(RenderCommandList& commandList, GpuMeshPreparedData& preparedData);
	void CommitPreparedData(GpuMeshPreparedData&& preparedData);

	RenderHardwareInterface* m_renderHardwareInterface = nullptr;
	GpuMeshHandle m_handle = {};
	GpuMeshBuffer m_vertexBuffer;
	GpuMeshBuffer m_indexBuffer;
	GpuMeshBuffer m_skinInfluences;
	GpuMeshBuffer m_morphTargets;
	RhiVertexBufferView m_vertexBufferView{};
	RhiIndexBufferView m_indexBufferView{};

	std::uint32_t m_vertexCount = 0;
	std::uint32_t m_indexCount = 0;
	GpuMeshBounds m_localBounds = {};
	std::vector<RayTracingHitVertex> m_rayTracingHitVertices;
	std::vector<std::uint32_t> m_rayTracingHitIndices;
	std::vector<VertexSkinInfluence> m_cpuSkinInfluences;
	std::vector<MorphTargetDeltaData> m_morphTargetDeltas;
	std::uint32_t m_morphTargetCount = 0;
};
