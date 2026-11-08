#pragma once

#include "Vulkan/VulkanIncludes.h"

#include <cstdint>
#include <span>

struct CompiledBinding;

enum class VulkanDescriptorEntryKind : std::uint8_t
{
	Empty,
	SampledImage,
	StorageImage,
	UniformBuffer,
	StorageBuffer,
	Sampler,
	AccelerationStructure,
	Ptlas,
};

struct VulkanDescriptorEntry final
{
	VulkanDescriptorEntryKind Kind = VulkanDescriptorEntryKind::Empty;
	VkDescriptorImageInfo Image = {};
	VkDescriptorBufferInfo Buffer = {};
	VkAccelerationStructureKHR AccelerationStructure = VK_NULL_HANDLE;
	VkDeviceAddress PtlasAddress = 0;
};

namespace VulkanDescriptorWrites
{
	VkImageLayout ToImageLayout(VulkanDescriptorEntryKind kind) noexcept;
	void Write(VkDevice device, VkDescriptorSet descriptorSet, const CompiledBinding& binding, std::span<const VulkanDescriptorEntry> entries) noexcept;
}
