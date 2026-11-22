#pragma once

#include "Memory/RhiMemoryCategoryAggregation.h"
#include "Vulkan/VulkanIncludes.h"

#include <cstdint>
#include <span>
#include <vector>

struct VmaAllocator_T;
struct VmaBudget;
struct VulkanGpuAllocationRecord;
struct VulkanGpuMemoryBlockRecord;

struct VulkanMemoryBlockIdentity final
{
	VkDeviceMemory Memory = VK_NULL_HANDLE;
	std::uint32_t HeapIndex = 0;

	bool operator==(const VulkanMemoryBlockIdentity&) const = default;
};

using VulkanMemoryCategoryAggregation = RhiMemoryCategoryAggregation<VulkanMemoryBlockIdentity>;

namespace VulkanMemoryUsage
{
	void QueryBudget(VmaAllocator_T* allocator, std::span<VmaBudget, VK_MAX_MEMORY_HEAPS> heapBudgets, RhiMemoryUsageSnapshot& snapshot);
	// Live record spans remain borrowed under the allocator's records mutex.
	void AccumulateAllocations(
	    VmaAllocator_T* allocator,
	    std::span<VulkanGpuAllocationRecord* const> records,
	    std::span<const VmaBudget> heapBudgets,
	    std::vector<VulkanMemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot);
	void AccumulateBlocks(
	    VmaAllocator_T* allocator,
	    std::span<VulkanGpuMemoryBlockRecord* const> records,
	    std::span<const VmaBudget> heapBudgets,
	    std::vector<VulkanMemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot);
	void AppendCategories(std::span<const VulkanMemoryCategoryAggregation> aggregations, RhiMemoryUsageSnapshot& snapshot);
}
