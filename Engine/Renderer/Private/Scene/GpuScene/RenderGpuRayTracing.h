#pragma once

#include "RHI/Public/Frame/RhiFrameConstants.h"
#include "Scene/GpuScene/PersistentStructuredBuffer.h"
#include "Scene/GpuScene/RenderSceneGpuBindings.h"

#include <array>

class RhiResourceService;
#include "Scene/GpuScene/RenderGpuRayTracingPayloadBuilder.h"

#include <limits>

class RenderGpuRayTracing final
{
public:
	RenderGpuRayTracing(RhiResourceService& resources, const GpuMeshCache& meshes) noexcept;
	RenderSceneGpuRayTracingBindings Update(const PreparedRenderScene& preparedScene, std::uint32_t frameIndex);
	void Reset() noexcept;

private:
	struct FrameStorage final
	{
		PersistentStructuredBuffer RayTracingInstances;
		PersistentStructuredBuffer RayTracingMaterials;
		std::uint64_t PayloadRevision = 0u;
	};

	struct TopologyStorage final
	{
		PersistentStructuredBuffer Vertices;
		PersistentStructuredBuffer SkinInfluences;
		PersistentStructuredBuffer MorphTargetDeltas;
		PersistentStructuredBuffer Indices;
	};

	void RefreshPayloads(const PreparedRenderScene& preparedScene);
	void UpdateTopology();
	void UploadFrame(FrameStorage& storage);
	RenderSceneGpuRayTracingBindings PublishBindings(FrameStorage& storage);
	TopologyStorage m_topology;
	RhiResourceService& m_resourceService;
	const GpuMeshCache& m_meshes;
	RenderGpuRayTracingPayloads m_payloads;
	std::uint64_t m_payloadRevision = 0u;
	std::uint64_t m_structuralRevision = (std::numeric_limits<std::uint64_t>::max)();
	std::uint64_t m_workTopologyHash = (std::numeric_limits<std::uint64_t>::max)();
	std::uint64_t m_materialRevision = (std::numeric_limits<std::uint64_t>::max)();
	std::uint64_t m_textureGeneration = (std::numeric_limits<std::uint64_t>::max)();
	std::array<FrameStorage, RhiFrameConstants::MaxFrameSlotCount> m_frames;
};
