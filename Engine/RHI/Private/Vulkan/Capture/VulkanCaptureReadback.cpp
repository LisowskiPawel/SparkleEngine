#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Capture/VulkanCaptureReadback.h"

#include "Capture/RhiCaptureFormat.h"
#include "Vulkan/Commands/VulkanCommandQueue.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/VulkanTypeConversions.h"

#include <cstring>

static std::uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, std::uint32_t typeBits, VkMemoryPropertyFlags requiredFlags) noexcept
{
	VkPhysicalDeviceMemoryProperties properties{};
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);
	for (std::uint32_t index = 0; index < properties.memoryTypeCount; ++index)
	{
		const bool typeAvailable = (typeBits & (1u << index)) != 0;
		const bool flagsMatch = (properties.memoryTypes[index].propertyFlags & requiredFlags) == requiredFlags;
		if (typeAvailable && flagsMatch)
		{
			return index;
		}
	}
	return UINT32_MAX;
}

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

VulkanCaptureService::PendingReadback::~PendingReadback() noexcept
{
	if (Device == VK_NULL_HANDLE)
	{
		return;
	}
	if (CommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(Device, CommandPool, nullptr);
	}
	if (Memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(Device, Memory, nullptr);
	}
	if (Buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(Device, Buffer, nullptr);
	}
}

std::unique_ptr<VulkanCaptureService::PendingReadback> VulkanCaptureService::PrepareReadback(const RhiTextureCaptureRequest& request) noexcept
{
	if (!request.Resource || request.Width == 0 || request.Height == 0)
	{
		return {};
	}

	const VkDevice device = m_rhi.GetDevice();
	const VkPhysicalDevice physicalDevice = m_rhi.GetPhysicalDevice();
	const VkImage sourceImage = static_cast<VkImage>(request.Resource.Value);
	if (device == VK_NULL_HANDLE || physicalDevice == VK_NULL_HANDLE || sourceImage == VK_NULL_HANDLE || !IsRhiCaptureFormatSupported(request.SourceFormat))
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

bool VulkanCaptureService::AllocateReadbackBuffer(PendingReadback& pending) noexcept
{
	const VkDevice device = pending.Device;
	const VkPhysicalDevice physicalDevice = m_rhi.GetPhysicalDevice();
	const VkBufferCreateInfo bufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, .size = pending.ByteCount, .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT, .sharingMode = VK_SHARING_MODE_EXCLUSIVE};
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &pending.Buffer) != VK_SUCCESS || pending.Buffer == VK_NULL_HANDLE)
	{
		return false;
	}

	VkMemoryRequirements requirements{};
	vkGetBufferMemoryRequirements(device, pending.Buffer, &requirements);
	const std::uint32_t memoryType = FindMemoryType(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if (memoryType == UINT32_MAX)
	{
		return false;
	}
	const VkMemoryAllocateInfo allocation{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, .allocationSize = requirements.size, .memoryTypeIndex = memoryType};
	if (vkAllocateMemory(device, &allocation, nullptr, &pending.Memory) != VK_SUCCESS || pending.Memory == VK_NULL_HANDLE
	    || vkBindBufferMemory(device, pending.Buffer, pending.Memory, 0) != VK_SUCCESS)
	{
		return false;
	}

	return true;
}

bool VulkanCaptureService::BeginReadbackCommands(PendingReadback& pending) noexcept
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

bool VulkanCaptureService::RecordReadbackCopy(PendingReadback& pending) noexcept
{
	const RhiTextureCaptureRequest& request = pending.Request;
	const VkImage sourceImage = static_cast<VkImage>(request.Resource.Value);
	RecordTransition(pending.CommandBuffer, sourceImage, request.SourceState, ResourceState::CopySource);

	const VkBufferImageCopy copyRegion{
	    .imageSubresource = VkImageSubresourceLayers{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
	    .imageExtent = VkExtent3D{.width = request.Width, .height = request.Height, .depth = 1}};

	vkCmdCopyImageToBuffer(pending.CommandBuffer, sourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pending.Buffer, 1, &copyRegion);
	RecordTransition(pending.CommandBuffer, sourceImage, ResourceState::CopySource, request.SourceState);
	if (vkEndCommandBuffer(pending.CommandBuffer) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

bool VulkanCaptureService::SubmitReadback(PendingReadback& pending) noexcept
{
	pending.Submission = m_rhi.GetCommandQueue(ERhiQueueType::Graphics).Submit(VulkanQueueSubmission{.CommandBuffers = std::span<const VkCommandBuffer>(&pending.CommandBuffer, 1)});
	return pending.Submission.IsValid();
}

void VulkanCaptureService::ReadPixels(PendingReadback& pending, RhiCaptureReadback& readback) noexcept
{
	void* mappedData = nullptr;
	if (vkMapMemory(pending.Device, pending.Memory, 0, pending.ByteCount, 0, &mappedData) != VK_SUCCESS || mappedData == nullptr)
	{
		readback.Result = RhiCaptureResult{
		    .Status = ERhiCaptureStatus::Failed,
		    .BackendApi = ERhiBackendApi::Vulkan,
		    .FrameId = pending.Request.FrameId,
		    .FailureReason = "Vulkan texture readback memory could not be mapped"};

		return;
	}
	readback.Result = RhiCaptureResult{.Status = ERhiCaptureStatus::Succeeded, .BackendApi = ERhiBackendApi::Vulkan, .FrameId = pending.Request.FrameId};
	readback.Width = pending.Request.Width;
	readback.Height = pending.Request.Height;
	readback.RowPitch = pending.RowPitch;
	readback.Format = pending.Request.SourceFormat;
	readback.Pixels.resize(static_cast<std::size_t>(pending.ByteCount));
	std::memcpy(readback.Pixels.data(), mappedData, readback.Pixels.size());
	vkUnmapMemory(pending.Device, pending.Memory);
}

bool VulkanCaptureService::IsReadbackComplete(const PendingReadback& pending) const noexcept
{
	return m_rhi.GetCommandQueue(ERhiQueueType::Graphics).IsSubmissionComplete(pending.Submission.Value);
}

void VulkanCaptureService::WaitForReadback(const PendingReadback& pending) noexcept
{
	m_rhi.GetCommandQueue(ERhiQueueType::Graphics).WaitForSubmission(pending.Submission.Value);
}
