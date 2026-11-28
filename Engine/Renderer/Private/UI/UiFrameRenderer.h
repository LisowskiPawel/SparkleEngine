#pragma once

#include "Renderer/Public/UI/UiTextureHandle.h"
#include "RHI/Public/Interop/ResourceState.h"

#include <cstdint>
#include <memory>

class FrameGraph;
class RenderDeviceServices;
class UiRenderPacketPlayer;
struct UiRenderPacket;
class ViewportRenderProducts;

class UiFrameRenderer final
{
public:
	UiFrameRenderer(RenderDeviceServices& deviceServices, bool bOwnsBackend);
	~UiFrameRenderer() noexcept;

	UiFrameRenderer(const UiFrameRenderer&) = delete;
	UiFrameRenderer& operator=(const UiFrameRenderer&) = delete;


	UiTextureHandle GetViewportTexture() const noexcept { return m_viewportTexture; }

	void BeginFrame() noexcept;
	void Render(const UiRenderPacket& packet, FrameGraph* frameGraph, ViewportRenderProducts& viewportProducts) noexcept;

private:
	bool BeginViewportPresentation(FrameGraph& frameGraph, ViewportRenderProducts& viewportProducts) noexcept;
	void EndViewportPresentation(FrameGraph& frameGraph, const ViewportRenderProducts& viewportProducts) noexcept;
	void RenderViewport(const UiRenderPacket& packet, FrameGraph* frameGraph, ViewportRenderProducts& viewportProducts) noexcept;
	void RenderHostOverlay(const UiRenderPacket& packet) noexcept;
	void Play(const UiRenderPacket& packet) noexcept;
	void TransitionViewportProduct(FrameGraph& frameGraph, const ViewportRenderProducts& viewportProducts, ResourceState resourceState) noexcept;

	RenderDeviceServices& m_deviceServices;
	std::unique_ptr<UiRenderPacketPlayer> m_packetPlayer;
	UiTextureHandle m_viewportTexture;
	std::uint64_t m_viewportTextureId = 0;
	bool m_bOwnsBackend = false;
};
