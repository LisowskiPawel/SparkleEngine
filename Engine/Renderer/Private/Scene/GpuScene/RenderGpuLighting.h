#pragma once

#include "RHI/Public/Frame/RhiFrameConstants.h"
#include "Scene/GpuScene/PersistentStructuredBuffer.h"
#include "Scene/GpuScene/RenderSceneGpuBindings.h"
#include "Scene/GpuScene/RenderGpuLightingPayloadBuilder.h"

#include <array>

class RhiResourceService;

class RenderGpuLighting final
{
public:
	explicit RenderGpuLighting(RhiResourceService& resources) noexcept;
	RenderSceneGpuLightingBindings Update(const PreparedRenderScene& preparedScene, std::uint32_t frameIndex);
	void Reset() noexcept;

private:
	struct FrameStorage final
	{
		PersistentStructuredBuffer DirectionalLights;
		PersistentStructuredBuffer PointLights;
		PersistentStructuredBuffer SpotLights;
		PersistentStructuredBuffer RectLights;
	};

	RhiResourceService& m_resourceService;
	RenderGpuLightingPayloads m_payloads;
	std::array<FrameStorage, RhiFrameConstants::MaxFrameSlotCount> m_frames;
};
