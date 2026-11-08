#include "PCH.h"

#include "D3D12/Memory/D3D12MemoryUsage.h"
#include "D3D12/Memory/D3D12GpuAllocation.h"

#include <D3D12MemAlloc.h>

#include <cstdint>

static std::uint64_t GetBudgetBytesForResidency(RhiMemoryResidencyClass residencyClass, const D3D12MA::Budget& localBudget, const D3D12MA::Budget& nonLocalBudget) noexcept
{
	switch (residencyClass)
	{
		case RhiMemoryResidencyClass::HostUpload:
		case RhiMemoryResidencyClass::HostReadback:
			return nonLocalBudget.BudgetBytes != 0 ? nonLocalBudget.BudgetBytes : localBudget.BudgetBytes;
		case RhiMemoryResidencyClass::DeviceLocal:
		case RhiMemoryResidencyClass::Transient:
		default:
			return localBudget.BudgetBytes;
	}
}

static D3D12MemoryCategoryAggregation& FindOrCreateAggregation(
    std::vector<D3D12MemoryCategoryAggregation>& aggregations,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    const D3D12MA::Budget& localBudget,
    const D3D12MA::Budget& nonLocalBudget)
{
	return RhiMemoryCategoryAggregationPolicy::FindOrCreate(aggregations, category, residencyClass, GetBudgetBytesForResidency(residencyClass, localBudget, nonLocalBudget));
}

static void AddBlockReference(D3D12MemoryCategoryAggregation& aggregation, D3D12MA::Allocation* allocation) noexcept
{
	if (allocation == nullptr)
	{
		return;
	}

	ID3D12Heap* const heap = allocation->GetHeap();
	if (heap == nullptr)
	{
		++aggregation.Stats.BlockCount;
		return;
	}

	RhiMemoryCategoryAggregationPolicy::AddUniqueBlock(aggregation, heap);
}

void D3D12MemoryUsage::QueryBudget(D3D12MA::Allocator& allocator, D3D12MA::Budget& localBudget, D3D12MA::Budget& nonLocalBudget, RhiMemoryUsageSnapshot& snapshot)
{
	allocator.GetBudget(&localBudget, &nonLocalBudget);

	D3D12MA::TotalStatistics totalStats = {};
	allocator.CalculateStatistics(&totalStats);
	snapshot.AllocatorBackend = ERhiMemoryAllocatorBackend::D3D12Managed;
	snapshot.bHasBudgetData = true;
	snapshot.bHasDelayedDestructionTracking = false;
	snapshot.TotalUsedBytes = totalStats.Total.Stats.AllocationBytes;
	snapshot.TotalAllocatedBytes = totalStats.Total.Stats.BlockBytes;
	snapshot.TotalBudgetBytes = localBudget.BudgetBytes + nonLocalBudget.BudgetBytes;
	snapshot.ApiUsageBytes = localBudget.UsageBytes + nonLocalBudget.UsageBytes;
}

void D3D12MemoryUsage::AccumulateAllocations(
    std::span<D3D12GpuAllocationRecord* const> records,
    const D3D12MA::Budget& localBudget,
    const D3D12MA::Budget& nonLocalBudget,
    std::vector<D3D12MemoryCategoryAggregation>& aggregations,
    RhiMemoryUsageSnapshot& snapshot)
{
	for (const D3D12GpuAllocationRecord* record : records)
	{
		if (record == nullptr || record->Allocation == nullptr)
		{
			continue;
		}

		const std::uint64_t allocationBytes = record->Allocation->GetSize();
		if (record->Allocation->GetHeap() == nullptr)
		{
			snapshot.CommittedUsageBytes += allocationBytes;
		}
		else
		{
			snapshot.PlacedUsageBytes += allocationBytes;
		}
		if (record->ResidencyClass == RhiMemoryResidencyClass::Transient || record->Category == RhiMemoryCategory::TransientResource)
		{
			snapshot.TransientUsageBytes += allocationBytes;
		}

		D3D12MemoryCategoryAggregation& aggregation = FindOrCreateAggregation(aggregations, record->Category, record->ResidencyClass, localBudget, nonLocalBudget);

		++aggregation.Stats.AllocationCount;
		if (record->Resource != nullptr)
		{
			++aggregation.Stats.ResourceCount;
		}
		aggregation.Stats.UsedBytes += allocationBytes;
		aggregation.Stats.AllocatedBytes += allocationBytes;
		AddBlockReference(aggregation, record->Allocation);
	}
}

void D3D12MemoryUsage::AccumulateHeaps(
    std::span<D3D12GpuHeapRecord* const> records,
    const D3D12MA::Budget& localBudget,
    const D3D12MA::Budget& nonLocalBudget,
    std::vector<D3D12MemoryCategoryAggregation>& aggregations,
    RhiMemoryUsageSnapshot& snapshot)
{
	for (const D3D12GpuHeapRecord* record : records)
	{
		if (record == nullptr || record->Allocation == nullptr)
		{
			continue;
		}

		const std::uint64_t allocationBytes = record->Allocation->GetSize();
		snapshot.PlacedUsageBytes += allocationBytes;
		if (record->ResidencyClass == RhiMemoryResidencyClass::Transient || record->Category == RhiMemoryCategory::TransientResource)
		{
			snapshot.TransientUsageBytes += allocationBytes;
		}

		D3D12MemoryCategoryAggregation& aggregation = FindOrCreateAggregation(aggregations, record->Category, record->ResidencyClass, localBudget, nonLocalBudget);

		++aggregation.Stats.AllocationCount;
		aggregation.Stats.ResourceCount += record->AliasingResourceCount;
		aggregation.Stats.UsedBytes += allocationBytes;
		aggregation.Stats.AllocatedBytes += allocationBytes;
		AddBlockReference(aggregation, record->Allocation);
	}
}

void D3D12MemoryUsage::AppendCategories(std::span<const D3D12MemoryCategoryAggregation> aggregations, RhiMemoryUsageSnapshot& snapshot)
{
	snapshot.CategoryStats.reserve(aggregations.size());
	for (const D3D12MemoryCategoryAggregation& aggregation : aggregations)
	{
		snapshot.CategoryStats.push_back(aggregation.Stats);
	}
}
