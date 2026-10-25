#include "PCH.h"
#include "Scene/GpuScene/RenderGpuRayTracing.h"

#include "Core/Public/Hash/HashUtils.h"
#include "Scene/Preparation/PreparedRenderScene.h"

RenderGpuRayTracing::RenderGpuRayTracing(RhiResourceService& resources, const GpuMeshCache& meshes) noexcept :
    m_resourceService(resources),
    m_meshes(meshes)
{
}

RenderSceneGpuRayTracingBindings RenderGpuRayTracing::Update(const PreparedRenderScene& preparedScene, std::uint32_t frameIndex)
{
	FrameStorage& storage = m_frames[frameIndex % m_frames.size()];
	RefreshPayloads(preparedScene);
	UploadFrame(storage);
	return PublishBindings(storage);
}

void RenderGpuRayTracing::RefreshPayloads(const PreparedRenderScene& preparedScene)
{
	const std::uint64_t textureGeneration = preparedScene.materialTextureTable.Generation;
	std::uint64_t topologyHash = Hash::ContinueFnv1a64Value(Hash::kFnv64OffsetBasis, preparedScene.rayTracingInstances.size());
	for (const RenderRayTracingInstance& input : preparedScene.rayTracingInstances)
	{
		topologyHash = Hash::ContinueFnv1a64Value(topologyHash, input.GpuSceneSlot);
	}
	topologyHash = Hash::FinalizeFnv1a64(topologyHash);
	const bool topologyChanged = preparedScene.structuralRevision != m_structuralRevision || topologyHash != m_workTopologyHash;
	const bool payloadChanged = topologyChanged || preparedScene.materialRevision != m_materialRevision || textureGeneration != m_textureGeneration;
	if (payloadChanged)
	{
		BuildRenderGpuRayTracingPayloads(preparedScene, m_meshes, m_payloads);
		if (topologyChanged || m_payloads.InstanceCount == 0u || !m_topology.Vertices.GetBinding() || !m_topology.MorphTargetDeltas.GetBinding())
		{
			UpdateTopology();
		}

		m_structuralRevision = preparedScene.structuralRevision;
		m_workTopologyHash = topologyHash;
		m_materialRevision = preparedScene.materialRevision;
		m_textureGeneration = textureGeneration;
		++m_payloadRevision;
	}
}

void RenderGpuRayTracing::UploadFrame(FrameStorage& storage)
{
	if (storage.PayloadRevision != m_payloadRevision)
	{
		storage.RayTracingInstances.Update(m_resourceService, std::span{m_payloads.Instances}, L"RayTracingHitInstances");
		storage.RayTracingMaterials.Update(m_resourceService, std::span{m_payloads.Materials}, L"RayTracingHitMaterials");
		storage.PayloadRevision = m_payloadRevision;
	}
}

RenderSceneGpuRayTracingBindings RenderGpuRayTracing::PublishBindings(FrameStorage& storage)
{
	return RenderSceneGpuRayTracingBindings{
	    .Vertices = m_topology.Vertices.GetBinding(),
	    .SkinInfluences = m_topology.SkinInfluences.GetBinding(),
	    .MorphTargetDeltas = m_topology.MorphTargetDeltas.GetBinding(),
	    .Indices = m_topology.Indices.GetBinding(),
	    .Instances = storage.RayTracingInstances.GetBinding(),
	    .Materials = storage.RayTracingMaterials.GetBinding(),
	    .InstanceCount = m_payloads.InstanceCount,
	    .MaterialCount = m_payloads.MaterialCount};
}

void RenderGpuRayTracing::UpdateTopology()
{
	m_topology.Vertices.Replace(m_resourceService, std::span{m_payloads.Vertices}, L"RayTracingHitVertices");
	m_topology.SkinInfluences.Replace(m_resourceService, std::span{m_payloads.SkinInfluences}, L"RayTracingHitSkinInfluences");
	m_topology.MorphTargetDeltas.Replace(m_resourceService, std::span{m_payloads.MorphTargetDeltas}, L"RayTracingHitMorphTargetDeltas");
	m_topology.Indices.Replace(m_resourceService, std::span{m_payloads.Indices}, L"RayTracingHitIndices");
}

void RenderGpuRayTracing::Reset() noexcept
{
	for (FrameStorage& storage : m_frames)
	{
		storage.RayTracingInstances.Reset();
		storage.RayTracingMaterials.Reset();
		storage.PayloadRevision = 0u;
	}
	m_topology.Vertices.Reset();
	m_topology.SkinInfluences.Reset();
	m_topology.MorphTargetDeltas.Reset();
	m_topology.Indices.Reset();
	m_payloads = {};
	m_payloadRevision = 0u;
	m_structuralRevision = (std::numeric_limits<std::uint64_t>::max)();
	m_workTopologyHash = (std::numeric_limits<std::uint64_t>::max)();
	m_materialRevision = (std::numeric_limits<std::uint64_t>::max)();
	m_textureGeneration = (std::numeric_limits<std::uint64_t>::max)();
}
