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

using VulkanMemoryCategoryAggregation = RhiMemoryCategoryAggregation<std::uint32_t>;

namespace VulkanMemoryUsage
{
	void QueryBudget(VmaAllocator_T* allocator, const VkPhysicalDeviceMemoryProperties& memoryProperties, std::span<VmaBudget, VK_MAX_MEMORY_HEAPS> heapBudgets, RhiMemoryUsageSnapshot& snapshot);
	// Live record spans remain borrowed under the allocator's records mutex.
	void AccumulateAllocations(
	    std::span<VulkanGpuAllocationRecord* const> records,
	    std::span<const VmaBudget> heapBudgets,
	    std::vector<VulkanMemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot);
	void AccumulateBlocks(
	    std::span<VulkanGpuMemoryBlockRecord* const> records,
	    std::span<const VmaBudget> heapBudgets,
	    std::vector<VulkanMemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot);
	void AppendCategories(std::span<const VulkanMemoryCategoryAggregation> aggregations, RhiMemoryUsageSnapshot& snapshot);
}
