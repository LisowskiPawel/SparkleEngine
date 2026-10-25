#include "PCH.h"
#include "Scene/GpuScene/RenderGpuLighting.h"

#include "Scene/Preparation/PreparedRenderScene.h"

RenderGpuLighting::RenderGpuLighting(RhiResourceService& resources) noexcept :
    m_resourceService(resources)
{
}

RenderSceneGpuLightingBindings RenderGpuLighting::Update(const PreparedRenderScene& preparedScene, std::uint32_t frameIndex)
{
	FrameStorage& storage = m_frames[frameIndex % m_frames.size()];
	BuildRenderGpuLightingPayloads(preparedScene, m_payloads);

	storage.DirectionalLights.Update(m_resourceService, std::span{m_payloads.DirectionalLights}, L"DirectionalLights");
	storage.PointLights.Update(m_resourceService, std::span{m_payloads.PointLights}, L"PointLights");
	storage.SpotLights.Update(m_resourceService, std::span{m_payloads.SpotLights}, L"SpotLights");
	storage.RectLights.Update(m_resourceService, std::span{m_payloads.RectLights}, L"RectLights");

	return RenderSceneGpuLightingBindings{
	    .Uniform = m_payloads.Uniform,
	    .DirectionalLights = storage.DirectionalLights.GetBinding(),
	    .PointLights = storage.PointLights.GetBinding(),
	    .SpotLights = storage.SpotLights.GetBinding(),
	    .RectLights = storage.RectLights.GetBinding()};
}

void RenderGpuLighting::Reset() noexcept
{
	for (FrameStorage& storage : m_frames)
	{
		storage.DirectionalLights.Reset();
		storage.PointLights.Reset();
		storage.SpotLights.Reset();
		storage.RectLights.Reset();
	}
	m_payloads = {};
}
