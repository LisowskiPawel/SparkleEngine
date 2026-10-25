#include "Vulkan/VulkanPCH.h"
#include "Vulkan/SwapChain/VulkanBackBufferPresentation.h"

#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/SwapChain/VulkanSwapChain.h"
#include "Vulkan/Descriptors/VulkanDescriptorService.h"
#include "Vulkan/Resources/VulkanResourceService.h"
#include "Vulkan/Commands/VulkanCommandRecordingContext.h"
#include "Vulkan/Commands/VulkanRenderCommandList.h"
#include "Vulkan/VulkanTypeConversions.h"

#include <algorithm>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_backBufferPresentationLogger, "RHI.Vulkan.Interface");

VulkanBackBufferPresentation::VulkanBackBufferPresentation(VulkanSwapChain& swapChain, VulkanDescriptorService& descriptors, VulkanResourceService& resources, const std::uint32_t& frameIndex) noexcept
    :
    m_swapChain(&swapChain),
    m_descriptorService(&descriptors),
    m_resourceService(&resources),
    m_currentFrameIndex(frameIndex)
{
}

void VulkanBackBufferPresentation::SetCommandRecordingContext(VulkanCommandRecordingContext& context) noexcept
{
	m_commandRecordingContext = &context;
}

RenderCommandList& VulkanBackBufferPresentation::GetGraphicsCommandList() noexcept
{
	m_resourceService->DrainCompletedResourceReleases();
	return m_commandRecordingContext->GetCurrentCommandList(ERhiQueueType::Graphics, m_currentFrameIndex);
}

RhiViewport VulkanBackBufferPresentation::GetBackBufferViewport() const noexcept
{
	return m_swapChain->GetDefaultViewport();
}

RhiRect VulkanBackBufferPresentation::GetBackBufferScissorRect() const noexcept
{
	return m_swapChain->GetDefaultScissorRect();
}

RhiCpuDescriptorHandle VulkanBackBufferPresentation::GetBackBufferRenderTargetView() const noexcept
{
	return m_descriptorService->GetResourceViewCpuHandle(GetCurrentBackBufferViewHandle());
}

RhiResourceHandle VulkanBackBufferPresentation::GetBackBufferResource() const noexcept
{
	return m_swapChain->GetCurrentBackBufferResource();
}

void VulkanBackBufferPresentation::BeginPresentRenderPass(RhiClearColorView clearColor) noexcept
{
	BeginCurrentBackBufferRendering(clearColor.data(), true);
}

void VulkanBackBufferPresentation::BeginPresentOverlayPass() noexcept
{
	BeginCurrentBackBufferRendering(nullptr, false);
}

void VulkanBackBufferPresentation::EndPresentRenderPass() noexcept
{
	EndCurrentBackBufferRendering();
}

PixelFormat VulkanBackBufferPresentation::GetPresentColorFormat() const noexcept
{
	return m_swapChain->GetBackBufferFormat();
}

RhiResourceViewHandle VulkanBackBufferPresentation::GetCurrentBackBufferViewHandle() const noexcept
{
	const std::uint32_t backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	return m_descriptorService->GetSwapChainBackBufferView(backBufferIndex);
}

static void RecordBackBufferRendering(VkCommandBuffer commandBuffer, VkImageView backBufferView, const RhiRect& scissorRect, const float* clearColor, bool clear) noexcept
{
	VkClearValue nativeClearValue = {};
	if (clear && clearColor != nullptr)
	{
		nativeClearValue.color.float32[0] = clearColor[0];
		nativeClearValue.color.float32[1] = clearColor[1];
		nativeClearValue.color.float32[2] = clearColor[2];
		nativeClearValue.color.float32[3] = clearColor[3];
	}

	const VkRenderingAttachmentInfo colorAttachment{
	    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
	    .pNext = nullptr,
	    .imageView = backBufferView,
	    .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .resolveMode = VK_RESOLVE_MODE_NONE,
	    .resolveImageView = VK_NULL_HANDLE,
	    .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	    .loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
	    .clearValue = nativeClearValue};

	const VkRenderingInfo renderingInfo{
	    .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
	    .pNext = nullptr,
	    .flags = 0,
	    .renderArea =
	        VkRect2D{
	            .offset = VkOffset2D{.x = scissorRect.Left, .y = scissorRect.Top},
	            .extent = VkExtent2D{.width = static_cast<std::uint32_t>(scissorRect.Right - scissorRect.Left), .height = static_cast<std::uint32_t>(scissorRect.Bottom - scissorRect.Top)}},
	    .layerCount = 1,
	    .viewMask = 0,
	    .colorAttachmentCount = 1,
	    .pColorAttachments = &colorAttachment,
	    .pDepthAttachment = nullptr,
	    .pStencilAttachment = nullptr};

	vkCmdBeginRendering(commandBuffer, &renderingInfo);
}

VulkanRenderCommandList& VulkanBackBufferPresentation::PrepareBackBufferRendering() noexcept
{
	if (m_commandRecordingContext == nullptr)
	{
		Diagnostics::Fatal(g_backBufferPresentationLogger, __FILE__, __LINE__, "Vulkan present rendering began before command recording was initialized.");
	}

	const VkImage backBuffer = m_swapChain->GetCurrentBackBufferImage();
	const VkImageView backBufferView = m_swapChain->GetCurrentBackBufferImageView();
	if (backBuffer == VK_NULL_HANDLE || backBufferView == VK_NULL_HANDLE)
	{
		Diagnostics::Fatal(g_backBufferPresentationLogger, __FILE__, __LINE__, "Vulkan present rendering has no acquired swap-chain image and view.");
	}

	auto& commandList = static_cast<VulkanRenderCommandList&>(GetGraphicsCommandList());
	commandList.SetViewport(GetBackBufferViewport());
	commandList.SetScissorRect(GetBackBufferScissorRect());
	commandList.SetRenderTarget(GetBackBufferRenderTargetView());

	const VkCommandBuffer commandBuffer = commandList.GetVulkanCommandBuffer();
	TransitionCurrentBackBuffer(commandBuffer, ResourceState::RenderTarget);

	return commandList;
}

void VulkanBackBufferPresentation::BeginCurrentBackBufferRendering(const float* clearColor, bool clear) noexcept
{
	VulkanRenderCommandList& commandList = PrepareBackBufferRendering();
	RecordBackBufferRendering(commandList.GetVulkanCommandBuffer(), m_swapChain->GetCurrentBackBufferImageView(), GetBackBufferScissorRect(), clearColor, clear);
}

void VulkanBackBufferPresentation::EndCurrentBackBufferRendering() noexcept
{
	if (m_commandRecordingContext == nullptr)
	{
		Diagnostics::Fatal(g_backBufferPresentationLogger, __FILE__, __LINE__, "Vulkan present rendering ended before command recording was initialized.");
	}

	const auto& commandList = static_cast<const VulkanRenderCommandList&>(m_commandRecordingContext->GetCurrentCommandList(ERhiQueueType::Graphics, m_currentFrameIndex));
	const VkCommandBuffer commandBuffer = commandList.GetVulkanCommandBuffer();
	vkCmdEndRendering(commandBuffer);
	TransitionCurrentBackBuffer(commandBuffer, ResourceState::Present);
}

void VulkanBackBufferPresentation::PrepareCurrentBackBufferForPresentation(VulkanRenderCommandList& commandList) noexcept
{
	const RhiResourceHandle backBuffer = m_swapChain->GetCurrentBackBufferResource();
	const std::span<const RhiResourceHandle> trackedResources = commandList.GetTrackedResources();
	const bool transitionedByCommandList = std::any_of(trackedResources.begin(), trackedResources.end(), [backBuffer](RhiResourceHandle resource) { return resource.Value == backBuffer.Value; });
	if (!transitionedByCommandList)
	{
		TransitionCurrentBackBuffer(commandList.GetVulkanCommandBuffer(), ResourceState::Present);
		return;
	}

	const std::uint32_t backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	if (backBufferIndex < m_swapChainBackBufferLayouts.size())
	{
		m_swapChainBackBufferLayouts[backBufferIndex] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return;
	}
	Diagnostics::Fatal(g_backBufferPresentationLogger, __FILE__, __LINE__, "Vulkan present preparation addressed an invalid swap-chain image index.");
}

void VulkanBackBufferPresentation::TransitionCurrentBackBuffer(VkCommandBuffer commandBuffer, ResourceState newState) noexcept
{
	if (commandBuffer == VK_NULL_HANDLE)
	{
		Diagnostics::Fatal(g_backBufferPresentationLogger, __FILE__, __LINE__, "Vulkan back-buffer transition has no command buffer.");
	}

	const std::uint32_t backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	if (backBufferIndex >= m_swapChainBackBufferLayouts.size())
	{
		Diagnostics::Fatal(g_backBufferPresentationLogger, __FILE__, __LINE__, "Vulkan back-buffer transition addressed an invalid swap-chain image index.");
	}

	const VulkanResourceStateMapping destinationState = VulkanTypeConversions::ToResourceStateMapping(newState);
	const VkImageLayout newLayout = destinationState.ImageLayout;
	VkImageLayout& currentLayout = m_swapChainBackBufferLayouts[backBufferIndex];
	if (currentLayout == newLayout)
	{
		return;
	}

	const ResourceState currentState = currentLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ? ResourceState::Present
	    : currentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL                     ? ResourceState::RenderTarget
	                                                                                    : ResourceState::Common;

	const VulkanResourceStateMapping sourceState = currentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VulkanResourceStateMapping{} : VulkanTypeConversions::ToResourceStateMapping(currentState);

	const VkImageMemoryBarrier2 imageBarrier{
	    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = sourceState.StageMask,
	    .srcAccessMask = sourceState.AccessMask,
	    .dstStageMask = destinationState.StageMask,
	    .dstAccessMask = destinationState.AccessMask,
	    .oldLayout = currentLayout,
	    .newLayout = newLayout,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .image = m_swapChain->GetCurrentBackBufferImage(),
	    .subresourceRange = VkImageSubresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

	const VkDependencyInfo dependencyInfo{
	    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
	    .pNext = nullptr,
	    .dependencyFlags = 0,
	    .memoryBarrierCount = 0,
	    .pMemoryBarriers = nullptr,
	    .bufferMemoryBarrierCount = 0,
	    .pBufferMemoryBarriers = nullptr,
	    .imageMemoryBarrierCount = 1,
	    .pImageMemoryBarriers = &imageBarrier};

	vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
	currentLayout = newLayout;
}

void VulkanBackBufferPresentation::RebuildSwapChainBackBufferViews() noexcept
{
	m_swapChainBackBufferLayouts.clear();
	const std::uint32_t backBufferCount = m_swapChain->GetBackBufferCount();
	m_swapChainBackBufferLayouts.assign(backBufferCount, VK_IMAGE_LAYOUT_UNDEFINED);
	m_descriptorService->RebuildSwapChainBackBufferViews(*m_swapChain);
}
