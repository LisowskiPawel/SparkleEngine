#pragma once

#include "Vulkan/VulkanIncludes.h"

#include <cstdint>
#include <span>
#include <vector>

class VulkanRhi;

class VulkanRecordingDescriptorPool final
{
public:
	explicit VulkanRecordingDescriptorPool(VulkanRhi& rhi) noexcept;
	~VulkanRecordingDescriptorPool() noexcept;

	VulkanRecordingDescriptorPool(const VulkanRecordingDescriptorPool&) = delete;
	VulkanRecordingDescriptorPool& operator=(const VulkanRecordingDescriptorPool&) = delete;
	VulkanRecordingDescriptorPool(VulkanRecordingDescriptorPool&&) = delete;
	VulkanRecordingDescriptorPool& operator=(VulkanRecordingDescriptorPool&&) = delete;

	void Reset() noexcept;
	VkDescriptorSet AllocateSet(VkDescriptorSetLayout layout, std::span<const VkDescriptorPoolSize> requirements) noexcept;

	VkDescriptorPool GetNativePool() const noexcept { return m_pages.front().Pool; }

private:
	static constexpr std::uint32_t DescriptorSetsPerPage = 256;

	struct PoolPage final
	{
		VkDescriptorPool Pool = VK_NULL_HANDLE;
		std::vector<VkDescriptorPoolSize> Capacity;
		std::vector<VkDescriptorPoolSize> Remaining;
		std::uint32_t AllocatedSets = 0;
	};

	void CreatePool(std::span<const VkDescriptorPoolSize> requirements) noexcept;
	static bool CanAllocate(const PoolPage& page, std::span<const VkDescriptorPoolSize> requirements) noexcept;
	VkDescriptorSet AllocateSet(PoolPage& page, VkDescriptorSetLayout layout, std::span<const VkDescriptorPoolSize> requirements) noexcept;

	VulkanRhi* m_rhi = nullptr;
	std::vector<PoolPage> m_pages;
};
