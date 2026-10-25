#include "PCH.h"
#include "Scene/GpuScene/RenderGpuGeometryBuffers.h"

#include "Scene/GpuScene/RenderGpuGeometryState.h"

RenderGpuGeometryBuffers::RenderGpuGeometryBuffers(RhiResourceService& resources) noexcept :
    m_resourceService(resources)
{
}

RenderSceneGpuGeometryBindings RenderGpuGeometryBuffers::Update(const RenderGpuGeometryState& geometry, std::uint32_t frameIndex)
{
	FrameStorage& storage = m_frames[frameIndex % m_frames.size()];
	const RenderGpuGeometryPayloads& payloads = geometry.GetPayloads();
	geometry.CollectMeshInstanceWriteRanges(storage.MeshInstanceRevision, m_writeRanges);

	storage.MeshInstances.UpdateRanges(
	    m_resourceService,
	    std::as_bytes(std::span<const MeshInstanceData>{payloads.MeshInstances}),
	    static_cast<std::uint32_t>(sizeof(MeshInstanceData)),
	    m_writeRanges,
	    L"MeshInstances");

	storage.MeshInstanceRevision = geometry.GetMeshInstanceRevision();

	if (storage.MeshInstanceSlotRevision != geometry.GetMeshInstanceSlotRevision())
	{
		storage.MeshInstanceSlots.Update(m_resourceService, std::span{payloads.MeshInstanceSlots}, L"MeshInstanceSlots");
		storage.MeshInstanceSlotRevision = geometry.GetMeshInstanceSlotRevision();
	}

	storage.JointMatrices.Update(m_resourceService, std::span{payloads.JointMatrices}, L"SkinningJointMatrices");
	storage.PreviousJointMatrices.Update(m_resourceService, std::span{payloads.PreviousJointMatrices}, L"PreviousSkinningJointMatrices");
	storage.MorphWeights.Update(m_resourceService, std::span{payloads.MorphWeights}, L"MorphWeights");
	storage.PreviousMorphWeights.Update(m_resourceService, std::span{payloads.PreviousMorphWeights}, L"PreviousMorphWeights");

	return RenderSceneGpuGeometryBindings{
	    .MeshInstances = storage.MeshInstances.GetBinding(),
	    .MeshInstanceSlots = storage.MeshInstanceSlots.GetBinding(),
	    .JointMatrices = storage.JointMatrices.GetBinding(),
	    .PreviousJointMatrices = storage.PreviousJointMatrices.GetBinding(),
	    .MorphWeights = storage.MorphWeights.GetBinding(),
	    .PreviousMorphWeights = storage.PreviousMorphWeights.GetBinding()};
}

void RenderGpuGeometryBuffers::Reset() noexcept
{
	for (FrameStorage& storage : m_frames)
	{
		storage.MeshInstances.Reset();
		storage.MeshInstanceSlots.Reset();
		storage.JointMatrices.Reset();
		storage.PreviousJointMatrices.Reset();
		storage.MorphWeights.Reset();
		storage.PreviousMorphWeights.Reset();
		storage.MeshInstanceRevision = 0u;
		storage.MeshInstanceSlotRevision = 0u;
	}
}
