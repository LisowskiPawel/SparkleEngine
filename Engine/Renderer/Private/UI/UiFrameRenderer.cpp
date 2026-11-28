#include "PCH.h"
#include "UI/UiFrameRenderer.h"

#include "Frame/Graph/RenderProductGraphHandle.h"
#include "FrameGraph/FrameGraph.h"
#include "Renderer/Public/UI/UiRenderPacket.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"
#include "RHI/Public/Commands/RenderCommandList.h"
#include "RHI/Public/Device/RenderDeviceServices.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/Presentation/RhiPresentationService.h"
#include "RHI/Public/UI/RhiImGuiRenderer.h"
#include "UI/UiRenderPacketPlayer.h"

UiFrameRenderer::UiFrameRenderer(RenderDeviceServices& deviceServices, bool bOwnsBackend) :
    m_deviceServices(deviceServices),
    m_packetPlayer(std::make_unique<UiRenderPacketPlayer>()),
    m_bOwnsBackend(bOwnsBackend)
{
	if (m_bOwnsBackend)
	{
		m_deviceServices.GetImGuiRenderer().Initialize();
	}
}

UiFrameRenderer::~UiFrameRenderer() noexcept
{
	if (m_bOwnsBackend)
	{
		m_packetPlayer->Shutdown(m_deviceServices.GetImGuiRenderer());
		m_deviceServices.GetImGuiRenderer().Shutdown();
	}
}

void UiFrameRenderer::BeginFrame() noexcept
{
	if (m_bOwnsBackend)
	{
		m_deviceServices.GetImGuiRenderer().BeginFrame();
	}
}

void UiFrameRenderer::Render(const UiRenderPacket& packet, FrameGraph* frameGraph, ViewportRenderProducts& viewportProducts) noexcept
{
	m_packetPlayer->SynchronizeTextures(packet, m_deviceServices.GetImGuiRenderer());

	if (packet.PresentationMode != UiPresentationMode::Viewport)
	{
		m_viewportTexture = {};
		m_viewportTextureId = 0;
	}
	switch (packet.PresentationMode)
	{
		case UiPresentationMode::HostOverlay:
			RenderHostOverlay(packet);
			break;
		case UiPresentationMode::Viewport:
			RenderViewport(packet, frameGraph, viewportProducts);
			break;
		case UiPresentationMode::None:
		default:
			break;
	}
}

bool UiFrameRenderer::BeginViewportPresentation(FrameGraph& frameGraph, ViewportRenderProducts& viewportProducts) noexcept
{
	const RenderProduct* product = viewportProducts.FindProduct(RenderOutputFlags::FinalColorLdr);
	if (product == nullptr || !product->Handle)
	{
		return false;
	}

	TransitionViewportProduct(frameGraph, viewportProducts, ResourceState::ShaderResource);
	const FrameGraphResourceHandle resource = ToFrameGraphResourceHandle(product->Handle);
	const RhiGpuDescriptorHandle shaderResourceView = frameGraph.ResolveShaderResourceView(FrameGraphTextureHandle{resource});
	const std::uint64_t textureId = m_deviceServices.GetImGuiRenderer().ResolveTextureId(shaderResourceView);
	const UiTextureHandle viewportTexture = UiTextureHandle::Viewport(viewportProducts.GetGeneration());
	if (textureId == 0u || !viewportTexture)
	{
		TransitionViewportProduct(frameGraph, viewportProducts, ResourceState::Common);
		return false;
	}

	m_viewportTexture = viewportTexture;
	m_viewportTextureId = textureId;
	return true;
}

void UiFrameRenderer::EndViewportPresentation(FrameGraph& frameGraph, const ViewportRenderProducts& viewportProducts) noexcept
{
	TransitionViewportProduct(frameGraph, viewportProducts, ResourceState::Common);
}

void UiFrameRenderer::RenderViewport(const UiRenderPacket& packet, FrameGraph* frameGraph, ViewportRenderProducts& viewportProducts) noexcept
{
	const bool bMatchingViewportGeneration = frameGraph != nullptr && packet.ViewportGeneration == viewportProducts.GetGeneration();
	const bool bPresentingViewport = bMatchingViewportGeneration && (BeginViewportPresentation(*frameGraph, viewportProducts));
	if (!bPresentingViewport)
	{
		m_viewportTexture = {};
		m_viewportTextureId = 0;
	}

	if (packet.HasDrawData())
	{
		constexpr float clearColor[4] = {0.06f, 0.06f, 0.07f, 1.0f};
		RhiPresentationService& presentation = m_deviceServices.GetRenderHardwareInterface().GetPresentationService();
		presentation.BeginPresentRenderPass(clearColor);
		Play(packet);
		presentation.EndPresentRenderPass();
	}
	if (bPresentingViewport)
	{
		EndViewportPresentation(*frameGraph, viewportProducts);
	}
}

void UiFrameRenderer::RenderHostOverlay(const UiRenderPacket& packet) noexcept
{
	if (!packet.HasDrawData())
	{
		return;
	}

	RhiPresentationService& presentation = m_deviceServices.GetRenderHardwareInterface().GetPresentationService();
	presentation.BeginPresentOverlayPass();
	Play(packet);
	presentation.EndPresentRenderPass();
}

void UiFrameRenderer::Play(const UiRenderPacket& packet) noexcept
{
	m_packetPlayer->Render(packet, m_viewportTexture, m_viewportTextureId, m_deviceServices.GetImGuiRenderer(), m_deviceServices.GetCurrentGraphicsCommandList());
}

void UiFrameRenderer::TransitionViewportProduct(FrameGraph& frameGraph, const ViewportRenderProducts& viewportProducts, ResourceState after) noexcept
{
	const RenderProduct* product = viewportProducts.FindProduct(RenderOutputFlags::FinalColorLdr);
	if (product == nullptr || !product->Handle)
	{
		return;
	}

	const FrameGraphResourceHandle resourceHandle = ToFrameGraphResourceHandle(product->Handle);
	const RhiResourceHandle resource = frameGraph.ResolveResource(FrameGraphTextureHandle{resourceHandle});
	if (!resource)
	{
		return;
	}

	const ResourceState before = frameGraph.GetTrackedResourceState(resourceHandle);
	if (before == after)
	{
		return;
	}

	RenderHardwareInterface& rhi = m_deviceServices.GetRenderHardwareInterface();
	RenderCommandList& commandList = m_deviceServices.GetGraphicsCommandList(rhi.GetCurrentFrameIndex());
	commandList.TransitionResource(resource, before, after);
	frameGraph.UpdateTrackedResourceState(resourceHandle, after);
}
