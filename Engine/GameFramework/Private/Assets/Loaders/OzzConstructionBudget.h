#pragma once

#include <cstdint>

namespace Assets
{
	class CookedAssetAllocationBudget;

	void ReserveOzzConstruction(CookedAssetAllocationBudget& budget, std::uint64_t jointCount, std::uint64_t keyCount);
}
