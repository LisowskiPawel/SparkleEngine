#include "PCH.h"
#include "World/ECS/EntityRegistry.h"

#include <limits>

namespace ECS
{
	EntityId EntityRegistry::Create()
	{
		if (!CanMutateStructure())
		{
			return EntityId::Invalid();
		}

		if (!m_freeSlots.empty())
		{
			const EntityId::Slot slot = m_freeSlots.back();
			m_freeSlots.pop_back();
			EntitySlot& entry = m_slots[slot];
			entry.bAlive = true;
			++m_liveCount;
			AdvanceStructureVersion();
			return EntityId(slot, entry.Generation);
		}

		if (m_slots.size() >= EntityId::InvalidSlot)
		{
			return EntityId::Invalid();
		}

		const EntityId::Slot slot = static_cast<EntityId::Slot>(m_slots.size());
		m_slots.push_back(EntitySlot{.Generation = 1, .bAlive = true});
		++m_liveCount;
		AdvanceStructureVersion();
		return EntityId(slot, 1);
	}

	bool EntityRegistry::Destroy(EntityId entity)
	{
		if (!CanMutateStructure() || !IsAlive(entity))
		{
			return false;
		}

		std::apply([entity](auto&... stores) { (stores.Remove(entity), ...); }, m_components);
		EntitySlot& entry = m_slots[entity.GetSlot()];
		entry.bAlive = false;
		--m_liveCount;
		AdvanceStructureVersion();

		if (entry.Generation == (std::numeric_limits<EntityId::Generation>::max)())
		{
			return true;
		}

		++entry.Generation;
		m_freeSlots.push_back(entity.GetSlot());
		return true;
	}

	bool EntityRegistry::IsAlive(EntityId entity) const noexcept
	{
		if (!entity.IsValid() || entity.GetSlot() >= m_slots.size())
		{
			return false;
		}
		const EntitySlot& entry = m_slots[entity.GetSlot()];
		return entry.bAlive && entry.Generation == entity.GetGeneration();
	}
}
