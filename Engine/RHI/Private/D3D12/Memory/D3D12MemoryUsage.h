#pragma once

#include "Memory/RhiMemoryCategoryAggregation.h"

#include <span>
#include <vector>

struct ID3D12Heap;
struct D3D12GpuAllocationRecord;
struct D3D12GpuHeapRecord;

namespace D3D12MA
{
	class Allocator;
	struct Budget;
}

using D3D12MemoryCategoryAggregation = RhiMemoryCategoryAggregation<ID3D12Heap*>;

namespace D3D12MemoryUsage
{
	void QueryBudget(D3D12MA::Allocator& allocator, D3D12MA::Budget& localBudget, D3D12MA::Budget& nonLocalBudget, RhiMemoryUsageSnapshot& snapshot);
	// Live records remain borrowed under the allocator's records mutex.
	void AccumulateAllocations(
	    std::span<D3D12GpuAllocationRecord* const> records,
	    const D3D12MA::Budget& localBudget,
	    const D3D12MA::Budget& nonLocalBudget,
	    std::vector<D3D12MemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot);
	void AccumulateHeaps(
	    std::span<D3D12GpuHeapRecord* const> records,
	    const D3D12MA::Budget& localBudget,
	    const D3D12MA::Budget& nonLocalBudget,
	    std::vector<D3D12MemoryCategoryAggregation>& aggregations,
	    RhiMemoryUsageSnapshot& snapshot);
	void AppendCategories(std::span<const D3D12MemoryCategoryAggregation> aggregations, RhiMemoryUsageSnapshot& snapshot);
}
