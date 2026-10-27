#include "PCH.h"

#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerSession.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Frame/RenderFrame.h"
#include "Scene/RayTracing/RenderRayTracingScene.h"
#include "View/RenderView.h"

ReferencePathTracerSession::ReferencePathTracerSession(RenderDeviceServices& deviceServices, RendererMemoryMonitor& memoryMonitor, RenderRayTracingScene& rayTracingScene) noexcept :
    m_deviceServices(deviceServices),
    m_rayTracingScene(rayTracingScene),
    m_resources(deviceServices, memoryMonitor)
{
}

bool ReferencePathTracerSession::PrepareFrame(const RenderFrame& frame, ViewportRenderAction action, std::uint64_t actionSequence) noexcept
{
	Update(frame, action, actionSequence, m_rayTracingScene.GetExecutionFrontend());
	return !m_selected || (m_unavailableReason == ViewportRenderProgressReason::None && m_resources.IsAllocated());
}

void ReferencePathTracerSession::PublishFrameProducts(ViewportFrameProducts& products) const noexcept
{
	products.Progress = m_selected ? GetProgress() : ViewportRenderProgress{};
	products.RadianceSamplePrefix = GetRadianceSamplePrefix();
}

void ReferencePathTracerSession::Update(const RenderFrame& frame, ViewportRenderAction action, std::uint64_t actionSequence, RayTracingExecutionFrontend executionFrontend) noexcept
{
	const RenderView& view = frame.View;
	CompletePendingCommit();
	if (!SelectViewport(view, actionSequence))
	{
		return;
	}

	ApplyAction(action, actionSequence, view);
	ApplyPendingTransfer(view);
	if (!AdmitViewport(view) || !ActivateViewport(frame, executionFrontend))
	{
		return;
	}

	PrepareFrameWork(view.renderExtent);
}

void ReferencePathTracerSession::ReserveGraphResources(FrameGraphBuilder& builder, RenderViewportExtent extent)
{
	m_resources.ReserveGraphResources(builder, extent);
}

bool ReferencePathTracerSession::BindResources(FrameGraphBuilder& builder) const noexcept
{
	return !m_selected || (m_unavailableReason == ViewportRenderProgressReason::None && m_resources.Bind(builder));
}
