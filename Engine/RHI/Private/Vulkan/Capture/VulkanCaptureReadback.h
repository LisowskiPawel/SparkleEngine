#pragma once

#include "Vulkan/Capture/VulkanCaptureService.h"
#include "Commands/RhiQueue.h"
#include "Vulkan/VulkanIncludes.h"

struct VulkanCaptureService::PendingReadback final
{
	PendingReadback() noexcept = default;
	~PendingReadback() noexcept;
	PendingReadback(const PendingReadback&) = delete;
	PendingReadback& operator=(const PendingReadback&) = delete;
	PendingReadback(PendingReadback&&) = delete;
	PendingReadback& operator=(PendingReadback&&) = delete;

	VkDevice Device = VK_NULL_HANDLE;
	RhiCaptureTicket Ticket;
	RhiTextureCaptureRequest Request;
	RhiSubmissionToken Submission;
	VkBuffer Buffer = VK_NULL_HANDLE;
	VkDeviceMemory Memory = VK_NULL_HANDLE;
	VkCommandPool CommandPool = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	std::uint64_t ByteCount = 0;
	std::uint32_t RowPitch = 0;
	bool Cancelled = false;
};
