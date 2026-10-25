#pragma once

#include "Presentation/RhiPresentationService.h"
#include "Interop/ResourceState.h"
#include "Resources/RhiResourceView.h"
#include "Vulkan/VulkanIncludes.h"

#include <vector>

class RenderCommandList;
class VulkanRhi;
class VulkanSwapChain;
class VulkanDescriptorService;
class VulkanResourceService;
class VulkanCommandRecordingContext;
class VulkanRenderCommandList;

class VulkanBackBufferPresentation final
{
public:
	VulkanBackBufferPresentation(VulkanSwapChain& swapChain, VulkanDescriptorService& descriptors, VulkanResourceService& resources, const std::uint32_t& frameIndex) noexcept;
	VulkanBackBufferPresentation(const VulkanBackBufferPresentation&) = delete;
	VulkanBackBufferPresentation& operator=(const VulkanBackBufferPresentation&) = delete;
	VulkanBackBufferPresentation(VulkanBackBufferPresentation&&) = delete;
	VulkanBackBufferPresentation& operator=(VulkanBackBufferPresentation&&) = delete;

	void SetCommandRecordingContext(VulkanCommandRecordingContext& context) noexcept;
	void RebuildSwapChainBackBufferViews() noexcept;
	void PrepareCurrentBackBufferForPresentation(VulkanRenderCommandList& commandList) noexcept;
	void TransitionCurrentBackBuffer(VkCommandBuffer commandBuffer, ResourceState newState) noexcept;

	RhiViewport GetBackBufferViewport() const noexcept;
	RhiRect GetBackBufferScissorRect() const noexcept;
	RhiCpuDescriptorHandle GetBackBufferRenderTargetView() const noexcept;
	RhiResourceHandle GetBackBufferResource() const noexcept;
	void BeginPresentRenderPass(RhiClearColorView clearColor) noexcept;
	void BeginPresentOverlayPass() noexcept;
	void EndPresentRenderPass() noexcept;
	PixelFormat GetPresentColorFormat() const noexcept;

private:
	RhiResourceViewHandle GetCurrentBackBufferViewHandle() const noexcept;
	VulkanRenderCommandList& PrepareBackBufferRendering() noexcept;
	void BeginCurrentBackBufferRendering(const float* clearColor, bool clear) noexcept;
	void EndCurrentBackBufferRendering() noexcept;
	RenderCommandList& GetGraphicsCommandList() noexcept;

	VulkanSwapChain* m_swapChain;
	VulkanDescriptorService* m_descriptorService;
	VulkanResourceService* m_resourceService;
	VulkanCommandRecordingContext* m_commandRecordingContext = nullptr;
	const std::uint32_t& m_currentFrameIndex;
	std::vector<VkImageLayout> m_swapChainBackBufferLayouts;
};
