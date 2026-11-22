#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Memory/VulkanMemoryUsage.h"
#include "Vulkan/Memory/VulkanGpuAllocation.h"

#include <vk_mem_alloc.h>
#include <algorithm>

namespace VulkanMemoryUsage
{
	void QueryBudget(VmaAllocator_T* allocator, std::span<VmaBudget, VK_MAX_MEMORY_HEAPS> heapBudgets, RhiMemoryUsageSnapshot& snapshot)
	{
		const VkPhysicalDeviceMemoryProperties* memoryProperties = nullptr;
		vmaGetMemoryProperties(allocator, &memoryProperties);
		vmaGetHeapBudgets(allocator, heapBudgets.data());
		snapshot.AllocatorBackend = ERhiMemoryAllocatorBackend::VulkanManaged;
		snapshot.bHasBudgetData = true;
		snapshot.bHasDelayedDestructionTracking = true;
		for (std::uint32_t heapIndex = 0; heapIndex < memoryProperties->memoryHeapCount; ++heapIndex)
		{
			snapshot.TotalUsedBytes += heapBudgets[heapIndex].statistics.allocationBytes;
			snapshot.TotalAllocatedBytes += heapBudgets[heapIndex].statistics.blockBytes;
			snapshot.TotalBudgetBytes += heapBudgets[heapIndex].budget;
			snapshot.ApiUsageBytes += heapBudgets[heapIndex].usage;
		}
	}

	static VulkanMemoryCategoryAggregation& AccumulateCategoryUsage(
	    VmaAllocator_T* allocator,
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
		VmaAllocationInfo allocationInfo{};
		vmaGetAllocationInfo(allocator, record.Allocation, &allocationInfo);
		const VkPhysicalDeviceMemoryProperties* memoryProperties = nullptr;
		vmaGetMemoryProperties(allocator, &memoryProperties);
		const std::uint32_t heapIndex = memoryProperties->memoryTypes[allocationInfo.memoryType].heapIndex;
		const bool bHeapCounted = std::any_of(
		    aggregation.UniqueBlocks.begin(),
		    aggregation.UniqueBlocks.end(),
		    [heapIndex](const VulkanMemoryBlockIdentity& block) noexcept { return block.HeapIndex == heapIndex; });

		if (heapIndex < heapBudgets.size())
		{
			const std::uint64_t budgetBytes = bHeapCounted ? 0 : heapBudgets[heapIndex].budget;
			const VulkanMemoryBlockIdentity block{.Memory = allocationInfo.deviceMemory, .HeapIndex = heapIndex};
			RhiMemoryCategoryAggregationPolicy::AddUniqueBlock(aggregation, block, budgetBytes);
		}
		return aggregation;
	}

	void AccumulateAllocations(
	    VmaAllocator_T* allocator,
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
			VulkanMemoryCategoryAggregation& aggregation = AccumulateCategoryUsage(allocator, *record, heapBudgets, aggregations, snapshot);
			++aggregation.Stats.ResourceCount;
			snapshot.CommittedUsageBytes += record->AllocatedBytes;
		}
	}

	void AccumulateBlocks(
	    VmaAllocator_T* allocator,
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
			VulkanMemoryCategoryAggregation& aggregation = AccumulateCategoryUsage(allocator, *record, heapBudgets, aggregations, snapshot);
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
