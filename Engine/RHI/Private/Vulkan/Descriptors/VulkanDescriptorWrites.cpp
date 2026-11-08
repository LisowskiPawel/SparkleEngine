#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Descriptors/VulkanDescriptorWrites.h"
#include "Pipeline/RhiPipelineDesc.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <algorithm>
#include <array>
#include <cstddef>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanDescriptorWriterLogger, "RHI.Vulkan.DescriptorAllocator");

static constexpr std::size_t DescriptorWriteChunkCapacity = 64;

struct DescriptorWriteChunk final
{
	std::array<VkDescriptorImageInfo, DescriptorWriteChunkCapacity> ImageInfos;
	std::array<VkDescriptorBufferInfo, DescriptorWriteChunkCapacity> BufferInfos;
	std::array<VkAccelerationStructureKHR, DescriptorWriteChunkCapacity> AccelerationStructures;
	std::array<VkDeviceAddress, DescriptorWriteChunkCapacity> PtlasAddresses;
	std::uint32_t Count = 0;
};

static VkDescriptorType ToDescriptorType(VulkanDescriptorEntryKind kind) noexcept
{
	switch (kind)
	{
		case VulkanDescriptorEntryKind::SampledImage:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case VulkanDescriptorEntryKind::StorageImage:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case VulkanDescriptorEntryKind::UniformBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case VulkanDescriptorEntryKind::StorageBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case VulkanDescriptorEntryKind::Sampler:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		case VulkanDescriptorEntryKind::AccelerationStructure:
			return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		case VulkanDescriptorEntryKind::Ptlas:
			return VK_DESCRIPTOR_TYPE_PARTITIONED_ACCELERATION_STRUCTURE_NV;
		case VulkanDescriptorEntryKind::Empty:
		default:
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

VkImageLayout VulkanDescriptorWrites::ToImageLayout(VulkanDescriptorEntryKind kind) noexcept
{
	return kind == VulkanDescriptorEntryKind::StorageImage ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

static bool EntryKindMatchesBinding(const CompiledBinding& binding, VulkanDescriptorEntryKind entryKind) noexcept
{
	switch (binding.SemanticKind)
	{
		case ShaderParameterSemanticKind::ReadTexture:
			return entryKind == VulkanDescriptorEntryKind::SampledImage;
		case ShaderParameterSemanticKind::ReadBuffer:
		case ShaderParameterSemanticKind::RWBuffer:
			return entryKind == VulkanDescriptorEntryKind::StorageBuffer;
		case ShaderParameterSemanticKind::RWTexture:
			return entryKind == VulkanDescriptorEntryKind::StorageImage;
		case ShaderParameterSemanticKind::UniformData:
			return entryKind == VulkanDescriptorEntryKind::UniformBuffer;
		case ShaderParameterSemanticKind::SamplerSet:
			return entryKind == VulkanDescriptorEntryKind::Sampler;
		case ShaderParameterSemanticKind::AccelerationStructure:
			return entryKind == VulkanDescriptorEntryKind::AccelerationStructure || entryKind == VulkanDescriptorEntryKind::Ptlas;
		default:
			return false;
	}
}

static bool BuildWriteChunk(std::span<const VulkanDescriptorEntry> entries, VulkanDescriptorEntryKind entryKind, DescriptorWriteChunk& outChunk) noexcept
{
	if (entries.empty() || entries.size() > DescriptorWriteChunkCapacity)
	{
		return false;
	}

	outChunk.Count = static_cast<std::uint32_t>(entries.size());
	for (std::size_t index = 0; index < entries.size(); ++index)
	{
		const VulkanDescriptorEntry& entry = entries[index];
		if (entry.Kind != entryKind)
		{
			return false;
		}

		switch (entryKind)
		{
			case VulkanDescriptorEntryKind::SampledImage:
			case VulkanDescriptorEntryKind::StorageImage:
			case VulkanDescriptorEntryKind::Sampler:
				outChunk.ImageInfos[index] = entry.Image;
				break;

			case VulkanDescriptorEntryKind::AccelerationStructure:
				outChunk.AccelerationStructures[index] = entry.AccelerationStructure;
				break;

			case VulkanDescriptorEntryKind::Ptlas:
				outChunk.PtlasAddresses[index] = entry.PtlasAddress;
				break;

			case VulkanDescriptorEntryKind::UniformBuffer:
			case VulkanDescriptorEntryKind::StorageBuffer:
				outChunk.BufferInfos[index] = entry.Buffer;
				break;
			case VulkanDescriptorEntryKind::Empty:
			default:
				return false;
		}
	}

	return true;
}

static void CommitWriteChunk(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    const CompiledBinding& binding,
    VulkanDescriptorEntryKind entryKind,
    VkDescriptorType descriptorType,
    std::uint32_t firstDescriptor,
    const DescriptorWriteChunk& chunk) noexcept
{
	const VkWriteDescriptorSetAccelerationStructureKHR accelerationStructureWrite{
	    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
	    .pNext = nullptr,
	    .accelerationStructureCount = chunk.Count,
	    .pAccelerationStructures = chunk.AccelerationStructures.data()};

	const VkWriteDescriptorSetPartitionedAccelerationStructureNV ptlasWrite{
	    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_PARTITIONED_ACCELERATION_STRUCTURE_NV,
	    .pNext = nullptr,
	    .accelerationStructureCount = chunk.Count,
	    .pAccelerationStructures = chunk.PtlasAddresses.data()};

	const bool bImageEntry = entryKind == VulkanDescriptorEntryKind::SampledImage || entryKind == VulkanDescriptorEntryKind::StorageImage;
	const bool bWritesImages = bImageEntry || (entryKind == VulkanDescriptorEntryKind::Sampler);
	const bool bWritesBuffers = entryKind == VulkanDescriptorEntryKind::UniformBuffer || entryKind == VulkanDescriptorEntryKind::StorageBuffer;

	const VkWriteDescriptorSet write{
	    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	    .pNext = entryKind == VulkanDescriptorEntryKind::AccelerationStructure ? static_cast<const void*>(&accelerationStructureWrite)
	                                                                           : (entryKind == VulkanDescriptorEntryKind::Ptlas ? static_cast<const void*>(&ptlasWrite) : nullptr),
	    .dstSet = descriptorSet,
	    .dstBinding = binding.BindingPoint.Binding,
	    .dstArrayElement = firstDescriptor,
	    .descriptorCount = chunk.Count,
	    .descriptorType = descriptorType,
	    .pImageInfo = bWritesImages ? chunk.ImageInfos.data() : nullptr,
	    .pBufferInfo = bWritesBuffers ? chunk.BufferInfos.data() : nullptr,
	    .pTexelBufferView = nullptr};

	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void VulkanDescriptorWrites::Write(VkDevice device, VkDescriptorSet descriptorSet, const CompiledBinding& binding, std::span<const VulkanDescriptorEntry> entries) noexcept
{
	if (descriptorSet == VK_NULL_HANDLE || entries.empty())
	{
		Diagnostics::Fatal(g_vulkanDescriptorWriterLogger, __FILE__, __LINE__, "Vulkan descriptor write requires a descriptor set and at least one descriptor.");
	}
	const bool bDescriptorCountDescriptorCount = entries.size() > binding.DescriptorCount;
	const bool bInvalidDescriptorCountBindless = !bDescriptorCountDescriptorCount && (!binding.Bindless.bBindlessEligible);
	const bool bFixedCountMismatch = bInvalidDescriptorCountBindless && (entries.size() != binding.DescriptorCount);
	const bool bInvalidDescriptorCount = bDescriptorCountDescriptorCount || bFixedCountMismatch;

	if (bInvalidDescriptorCount)
	{
		Diagnostics::Fatal(g_vulkanDescriptorWriterLogger, __FILE__, __LINE__, "Vulkan descriptor write cardinality does not match the compiled shader binding.");
	}

	const VulkanDescriptorEntryKind entryKind = entries.front().Kind;
	const VkDescriptorType descriptorType = ToDescriptorType(entryKind);
	if (descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM)
	{
		Diagnostics::Fatal(g_vulkanDescriptorWriterLogger, __FILE__, __LINE__, "Vulkan descriptor write contains an empty entry.");
	}

	if (!EntryKindMatchesBinding(binding, entryKind))
	{
		Diagnostics::Fatal(g_vulkanDescriptorWriterLogger, __FILE__, __LINE__, "Vulkan descriptor entry kind does not match the compiled shader binding.");
	}

	std::size_t firstDescriptor = 0;
	while (firstDescriptor < entries.size())
	{
		const std::size_t descriptorCount = std::min(DescriptorWriteChunkCapacity, entries.size() - firstDescriptor);
		const std::span<const VulkanDescriptorEntry> entryChunk = entries.subspan(firstDescriptor, descriptorCount);

		DescriptorWriteChunk chunk{};
		if (!BuildWriteChunk(entryChunk, entryKind, chunk))
		{
			Diagnostics::Fatal(g_vulkanDescriptorWriterLogger, __FILE__, __LINE__, "Vulkan descriptor array contains inconsistent or unsupported entries.");
		}

		CommitWriteChunk(device, descriptorSet, binding, entryKind, descriptorType, static_cast<std::uint32_t>(firstDescriptor), chunk);
		firstDescriptor += chunk.Count;
	}
}
