#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Capture/VulkanTextureReadback.h"

#include "Capture/RhiCaptureFormat.h"
#include "Vulkan/Commands/VulkanCommandQueue.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"
#include "Vulkan/VulkanTypeConversions.h"

VulkanTextureReadback::VulkanTextureReadback(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept :
    m_rhi(rhi),
    m_memoryAllocator(memoryAllocator)
{
}

VulkanTextureReadback::~VulkanTextureReadback() noexcept = default;

static void RecordTransition(VkCommandBuffer commandBuffer, VkImage image, ResourceState before, ResourceState after) noexcept
{
	if (commandBuffer == VK_NULL_HANDLE || image == VK_NULL_HANDLE || before == after)
	{
		return;
	}

	const VulkanResourceStateMapping source = VulkanTypeConversions::ToResourceStateMapping(before);
	const VulkanResourceStateMapping destination = VulkanTypeConversions::ToResourceStateMapping(after);

	const VkImageMemoryBarrier2 barrier{
	    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
	    .srcStageMask = source.StageMask,
	    .srcAccessMask = source.AccessMask,
	    .dstStageMask = destination.StageMask,
	    .dstAccessMask = destination.AccessMask,
	    .oldLayout = source.ImageLayout,
	    .newLayout = destination.ImageLayout,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .image = image,
	    .subresourceRange = VkImageSubresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

	const VkDependencyInfo dependency{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier};
	vkCmdPipelineBarrier2(commandBuffer, &dependency);
}

VulkanTextureReadback::PendingReadback::~PendingReadback() noexcept
{
	if (Device == VK_NULL_HANDLE)
	{
		return;
	}
	if (CommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(Device, CommandPool, nullptr);
	}
}

std::unique_ptr<VulkanTextureReadback::PendingReadback> VulkanTextureReadback::PrepareReadback(const RhiTextureCaptureRequest& request) noexcept
{
	if (!request.Resource || request.Width == 0 || request.Height == 0)
	{
		return {};
	}

	const VkDevice device = m_rhi.GetDevice();
	const VkPhysicalDevice physicalDevice = m_rhi.GetPhysicalDevice();
	const VkImage sourceImage = static_cast<VkImage>(request.Resource.Value);
	const bool bCaptureSourcePhysicalDevice = device == VK_NULL_HANDLE || physicalDevice == VK_NULL_HANDLE;
	const bool bInvalidCaptureSourceSourceImage = bCaptureSourcePhysicalDevice || (sourceImage == VK_NULL_HANDLE);
	const bool bInvalidCaptureSource = bInvalidCaptureSourceSourceImage || (!IsRhiCaptureFormatSupported(request.SourceFormat));

	if (bInvalidCaptureSource)
	{
		return {};
	}

	auto pending = std::make_unique<PendingReadback>();
	pending->Ticket = RhiCaptureTicket{m_nextTicket++};
	pending->Request = request;
	pending->RowPitch = request.Width * PixelFormatBytesPerTexel(request.SourceFormat);
	pending->ByteCount = static_cast<std::uint64_t>(pending->RowPitch) * request.Height;
	pending->Device = device;
	return pending;
}

bool VulkanTextureReadback::AllocateReadbackBuffer(PendingReadback& pending) noexcept
{
	const VkBufferCreateInfo bufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, .size = pending.ByteCount, .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT, .sharingMode = VK_SHARING_MODE_EXCLUSIVE};
	pending.Buffer = m_memoryAllocator.CreateBuffer(bufferInfo, RhiMemoryCategory::Readback, RhiMemoryResidencyClass::HostReadback, L"Texture capture readback");
	return pending.Buffer != nullptr;
}

bool VulkanTextureReadback::BeginReadbackCommands(PendingReadback& pending) noexcept
{
	const VkDevice device = pending.Device;
	const VkCommandPoolCreateInfo poolInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, .queueFamilyIndex = m_rhi.GetGraphicsQueueFamilyIndex()};
	if (vkCreateCommandPool(device, &poolInfo, nullptr, &pending.CommandPool) != VK_SUCCESS)
	{
		return false;
	}

	const VkCommandBufferAllocateInfo commandBufferInfo{
	    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	    .commandPool = pending.CommandPool,
	    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	    .commandBufferCount = 1};

	if (vkAllocateCommandBuffers(device, &commandBufferInfo, &pending.CommandBuffer) != VK_SUCCESS)
	{
		return false;
	}

	const VkCommandBufferBeginInfo beginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
	if (vkBeginCommandBuffer(pending.CommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		return false;
	}

	return true;
}

bool VulkanTextureReadback::RecordReadbackCopy(PendingReadback& pending) noexcept
{
	const RhiTextureCaptureRequest& request = pending.Request;
	const VkImage sourceImage = static_cast<VkImage>(request.Resource.Value);
	RecordTransition(pending.CommandBuffer, sourceImage, request.SourceState, ResourceState::CopySource);

	const VkBufferImageCopy copyRegion{
	    .imageSubresource = VkImageSubresourceLayers{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
	    .imageExtent = VkExtent3D{.width = request.Width, .height = request.Height, .depth = 1}};

	vkCmdCopyImageToBuffer(pending.CommandBuffer, sourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pending.Buffer->Buffer, 1, &copyRegion);
	RecordTransition(pending.CommandBuffer, sourceImage, ResourceState::CopySource, request.SourceState);
	if (vkEndCommandBuffer(pending.CommandBuffer) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

bool VulkanTextureReadback::SubmitReadback(PendingReadback& pending) noexcept
{
	const VulkanQueueSubmission submission{.CommandBuffers = std::span<const VkCommandBuffer>(&pending.CommandBuffer, 1)};
	pending.Submission = m_rhi.GetCommandQueue(ERhiQueueType::Graphics).Submit(submission);
	return pending.Submission.IsValid();
}

void VulkanTextureReadback::ReadPixels(PendingReadback& pending, RhiCaptureReadback& readback) noexcept
{
	readback.Pixels.resize(static_cast<std::size_t>(pending.ByteCount));
	const bool bPixelsRead = m_memoryAllocator.ReadAllocation(*pending.Buffer, readback.Pixels.data(), readback.Pixels.size());
	if (!bPixelsRead)
	{
		readback.Pixels.clear();
		readback.Result = RhiCaptureResult{
		    .Status = ERhiCaptureStatus::Failed,
		    .BackendApi = ERhiBackendApi::Vulkan,
		    .FrameId = pending.Request.FrameId,
		    .FailureReason = "Vulkan texture readback memory could not be copied to the CPU"};

		return;
	}
	readback.Result = RhiCaptureResult{.Status = ERhiCaptureStatus::Succeeded, .BackendApi = ERhiBackendApi::Vulkan, .FrameId = pending.Request.FrameId};
	readback.Width = pending.Request.Width;
	readback.Height = pending.Request.Height;
	readback.RowPitch = pending.RowPitch;
	readback.Format = pending.Request.SourceFormat;
}

bool VulkanTextureReadback::IsReadbackComplete(const PendingReadback& pending) const noexcept
{
	return m_rhi.GetCommandQueue(ERhiQueueType::Graphics).IsSubmissionComplete(pending.Submission.Value);
}

void VulkanTextureReadback::WaitForReadback(const PendingReadback& pending) noexcept
{
	m_rhi.GetCommandQueue(ERhiQueueType::Graphics).WaitForSubmission(pending.Submission.Value);
}
