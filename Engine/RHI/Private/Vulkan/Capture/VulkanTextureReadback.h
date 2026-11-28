#pragma once

#include "Capture/RhiCaptureService.h"
#include "Commands/RhiQueue.h"
#include "Vulkan/VulkanIncludes.h"
#include "Vulkan/Memory/VulkanGpuAllocation.h"

#include <cstdint>
#include <memory>

class VulkanRhi;
class VulkanGpuMemoryAllocator;

template <typename Implementation> class RhiCaptureServiceAdapter;

class VulkanTextureReadback final
{
public:
	explicit VulkanTextureReadback(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept;
	~VulkanTextureReadback() noexcept;

private:
	template <typename Implementation> friend class RhiCaptureServiceAdapter;
	struct PendingReadback;

	std::unique_ptr<PendingReadback> PrepareReadback(const RhiTextureCaptureRequest& request) noexcept;
	bool AllocateReadbackBuffer(PendingReadback& pending) noexcept;
	bool BeginReadbackCommands(PendingReadback& pending) noexcept;
	bool RecordReadbackCopy(PendingReadback& pending) noexcept;
	bool SubmitReadback(PendingReadback& pending) noexcept;
	void ReadPixels(PendingReadback& pending, RhiCaptureReadback& readback) noexcept;
	bool IsReadbackComplete(const PendingReadback& pending) const noexcept;
	void WaitForReadback(const PendingReadback& pending) noexcept;

	VulkanRhi& m_rhi;
	VulkanGpuMemoryAllocator& m_memoryAllocator;
	std::uint64_t m_nextTicket = 1;
};

struct VulkanTextureReadback::PendingReadback final
{
	PendingReadback() noexcept = default;
	~PendingReadback() noexcept;
	PendingReadback(const PendingReadback&) = delete;
	PendingReadback& operator=(const PendingReadback&) = delete;
	PendingReadback(PendingReadback&&) = delete;
	PendingReadback& operator=(PendingReadback&&) = delete;

	VulkanGpuMemoryAllocator* MemoryAllocator = nullptr;
	VkDevice Device = VK_NULL_HANDLE;
	RhiCaptureTicket Ticket;
	RhiTextureCaptureRequest Request;
	RhiSubmissionToken Submission;
	std::unique_ptr<VulkanGpuAllocationRecord> Buffer;
	VkCommandPool CommandPool = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	std::uint64_t ByteCount = 0;
	std::uint32_t RowPitch = 0;
	bool bCancelled = false;
};
