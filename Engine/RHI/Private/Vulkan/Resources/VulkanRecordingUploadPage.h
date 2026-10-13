#pragma once

#include "Resources/RhiResourceDesc.h"
#include "Vulkan/VulkanIncludes.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class VulkanGpuMemoryAllocator;
class VulkanRhi;

class VulkanRecordingUploadPage final
{
public:
	VulkanRecordingUploadPage(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator, std::uint64_t capacityInBytes) noexcept;
	~VulkanRecordingUploadPage() noexcept;

	VulkanRecordingUploadPage(const VulkanRecordingUploadPage&) = delete;
	VulkanRecordingUploadPage& operator=(const VulkanRecordingUploadPage&) = delete;
	VulkanRecordingUploadPage(VulkanRecordingUploadPage&&) = delete;
	VulkanRecordingUploadPage& operator=(VulkanRecordingUploadPage&&) = delete;

	void Reset() noexcept;
	RhiGpuVirtualAddress AllocateAndCopy(const void* data, std::uint32_t sizeInBytes) noexcept;
	bool Resolve(RhiGpuVirtualAddress address, VkBuffer& buffer, VkDeviceSize& offset, VkDeviceSize& range) const noexcept;

private:
	struct Page;
	void AddPage(std::uint64_t capacityInBytes);

	std::vector<std::unique_ptr<Page>> m_pages;
	VulkanGpuMemoryAllocator* m_memoryAllocator = nullptr;
	std::size_t m_pageIndex = 0;
	std::uint64_t m_capacityInBytes = 0;
	VkDeviceSize m_alignment = 1;
	VkDeviceSize m_offset = 0;
};
