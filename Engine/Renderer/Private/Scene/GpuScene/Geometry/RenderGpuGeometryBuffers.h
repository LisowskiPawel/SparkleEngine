#pragma once

#include "RHI/Public/Frame/RhiFrameConstants.h"
#include "Scene/GpuScene/PersistentStructuredBuffer.h"
#include "Scene/GpuScene/RenderSceneGpuBindings.h"

#include <array>
#include <vector>

class RhiResourceService;
class RenderGpuGeometryState;

class RenderGpuGeometryBuffers final
{
public:
	explicit RenderGpuGeometryBuffers(RhiResourceService& resources) noexcept;
	RenderSceneGpuGeometryBindings Update(const RenderGpuGeometryState& geometry, std::uint32_t frameIndex);
	void Reset() noexcept;

private:
	struct FrameStorage final
	{
		PersistentStructuredBuffer MeshInstances;
		PersistentStructuredBuffer MeshInstanceSlots;
		PersistentStructuredBuffer JointMatrices;
		PersistentStructuredBuffer PreviousJointMatrices;
		PersistentStructuredBuffer MorphWeights;
		PersistentStructuredBuffer PreviousMorphWeights;
		std::uint64_t MeshInstanceRevision = 0u;
		std::uint64_t MeshInstanceSlotRevision = 0u;
	};

	RhiResourceService& m_resourceService;
	std::vector<StructuredBufferElementRange> m_writeRanges;
	std::array<FrameStorage, RhiFrameConstants::MaxFrameSlotCount> m_frames;
};
