#pragma once

#include "Core/Public/Diagnostics/Error.h"

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace Assets
{
	// A load request admits its decoded allocations before construction. Charges remain
	// until settlement, including translated payloads retained for the final commit.
	class CookedAssetAllocationBudget final
	{
	public:
		explicit CookedAssetAllocationBudget(std::uint64_t maximumBytes = 2ull * 1024ull * 1024ull * 1024ull) noexcept :
		    m_maximumBytes(maximumBytes)
		{
		}

		void Reserve(std::size_t count, std::size_t elementSize = 1)
		{
			std::uint64_t used = m_reservedBytes.load(std::memory_order_relaxed);
			for (;;)
			{
				if (elementSize != 0 && count > (m_maximumBytes - used) / elementSize)
				{
					throw Diagnostics::Error("Scene load exceeds its decoded-allocation budget; the active world remains unchanged.");
				}
				const std::uint64_t next = used + count * elementSize;
				if (m_reservedBytes.compare_exchange_weak(used, next, std::memory_order_relaxed))
				{
					return;
				}
			}
		}

		template <typename T> void ReserveArray(std::size_t count) { Reserve(count, sizeof(T)); }

	private:
		const std::uint64_t m_maximumBytes;
		std::atomic<std::uint64_t> m_reservedBytes{0};
	};
}
