#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Memory/VulkanMemoryUsage.h"
#include "Vulkan/Memory/VulkanGpuAllocation.h"

#include <vk_mem_alloc.h>

namespace VulkanMemoryUsage
{
	void QueryBudget(VmaAllocator_T* allocator, const VkPhysicalDeviceMemoryProperties& memoryProperties, std::span<VmaBudget, VK_MAX_MEMORY_HEAPS> heapBudgets, RhiMemoryUsageSnapshot& snapshot)
	{
		vmaGetHeapBudgets(allocator, heapBudgets.data());
		VmaTotalStatistics totalStats = {};
		vmaCalculateStatistics(allocator, &totalStats);
		snapshot.AllocatorBackend = ERhiMemoryAllocatorBackend::VulkanManaged;
		snapshot.HasBudgetData = true;
		snapshot.HasDelayedDestructionTracking = true;
		snapshot.TotalUsedBytes = totalStats.total.statistics.allocationBytes;
		snapshot.TotalAllocatedBytes = totalStats.total.statistics.blockBytes;
		for (std::uint32_t heapIndex = 0; heapIndex < memoryProperties.memoryHeapCount; ++heapIndex)
		{
			snapshot.TotalBudgetBytes += heapBudgets[heapIndex].budget;
			snapshot.ApiUsageBytes += heapBudgets[heapIndex].usage;
		}
	}

	static VulkanMemoryCategoryAggregation& AccumulateCategoryUsage(
	    const auto& record,
	    std::span<const VmaBudget> heapBudgets,
	    std::vector<VulkanMemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot)
	{
		VulkanMemoryCategoryAggregation& aggregation = RhiMemoryCategoryAggregationPolicy::FindOrCreate(aggregations, record.Category, record.ResidencyClass);
		++aggregation.Stats.AllocationCount;
		aggregation.Stats.UsedBytes += record.UsedBytes;
		aggregation.Stats.AllocatedBytes += record.AllocatedBytes;
		if (record.ResidencyClass == RhiMemoryResidencyClass::Transient || record.Category == RhiMemoryCategory::TransientResource)
		{
			snapshot.TransientUsageBytes += record.AllocatedBytes;
		}
		if (record.MemoryHeapIndex < heapBudgets.size())
		{
			RhiMemoryCategoryAggregationPolicy::AddUniqueBlock(aggregation, record.MemoryHeapIndex, heapBudgets[record.MemoryHeapIndex].budget);
		}
		return aggregation;
	}

	void AccumulateAllocations(
	    std::span<VulkanGpuAllocationRecord* const> records,
	    std::span<const VmaBudget> heapBudgets,
	    std::vector<VulkanMemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot)
	{
		for (const VulkanGpuAllocationRecord* record : records)
		{
			if (record == nullptr || record->Allocation == nullptr || record->ParentMemoryBlock != nullptr)
			{
				continue;
			}
			VulkanMemoryCategoryAggregation& aggregation = AccumulateCategoryUsage(*record, heapBudgets, aggregations, snapshot);
			++aggregation.Stats.ResourceCount;
			snapshot.CommittedUsageBytes += record->AllocatedBytes;
		}
	}

	void AccumulateBlocks(
	    std::span<VulkanGpuMemoryBlockRecord* const> records,
	    std::span<const VmaBudget> heapBudgets,
	    std::vector<VulkanMemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot)
	{
		for (const VulkanGpuMemoryBlockRecord* record : records)
		{
			if (record == nullptr || record->Allocation == nullptr)
			{
				continue;
			}
			VulkanMemoryCategoryAggregation& aggregation = AccumulateCategoryUsage(*record, heapBudgets, aggregations, snapshot);
			aggregation.Stats.ResourceCount += record->AliasingResourceCount;
			snapshot.PlacedUsageBytes += record->AllocatedBytes;
		}
	}

	void AppendCategories(std::span<const VulkanMemoryCategoryAggregation> aggregations, RhiMemoryUsageSnapshot& snapshot)
	{
		snapshot.CategoryStats.reserve(aggregations.size());
		for (const VulkanMemoryCategoryAggregation& aggregation : aggregations)
		{
			snapshot.CategoryStats.push_back(aggregation.Stats);
		}
	}
}
