#include "PCH.h"

#include "Assets/Loaders/OzzConstructionBudget.h"

#include "Assets/Loaders/CookedAssetAllocationBudget.h"

namespace Assets
{
	void ReserveOzzConstruction(CookedAssetAllocationBudget& budget, std::uint64_t jointCount, std::uint64_t keyCount)
	{
		// Pinned 0.17.0 builders: raw vectors, sorted/padded keys, compressed output and
		// temporary sorting storage. Six endpoint/default keys per joint cover padding.
		// iframe_interval must remain zero: duration-dependent seek tables are excluded.
		// Reserve before constructing any raw SDK arrays; the SDK allocator is noexcept.
		budget.Reserve(32 * 1024);
		budget.Reserve(jointCount, 8 * 1024);
		budget.Reserve(jointCount, 6 * 1024);
		budget.Reserve(keyCount, 1024);
	}
}
