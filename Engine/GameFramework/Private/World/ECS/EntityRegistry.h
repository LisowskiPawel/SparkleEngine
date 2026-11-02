#pragma once

#include "GameFramework/Public/World/EntityId.h"
#include "World/ECS/ComponentTypeRegistry.h"
#include "World/ECS/StructureFrozenEpoch.h"

#include <cstdint>
#include <cstddef>
#include <utility>
#include <vector>

namespace ECS
{
	template <typename... AccessSpecs> class Query;

	class EntityRegistry final
	{
	public:
		EntityId Create();
		bool Destroy(EntityId entity);
		bool IsAlive(EntityId entity) const noexcept;

		std::size_t GetLiveCount() const noexcept { return m_liveCount; }

		std::uint64_t GetStructureVersion() const noexcept { return m_structureVersion; }

		bool IsStructureFrozen() const noexcept { return m_structureFrozen; }

		StructureFrozenEpoch FreezeStructure() noexcept;

		template <ComponentStorageCompatible T> bool Add(EntityId entity, T component)
		{
			if (!CanMutateStructure() || !IsAlive(entity))
			{
				return false;
			}
			const bool added = m_componentTypes.GetOrCreate<T>().Add(entity, std::move(component));
			if (added)
			{
				AdvanceStructureVersion();
			}
			return added;
		}

		template <ComponentStorageCompatible T> bool Replace(EntityId entity, T component)
		{
			ComponentStorage<T>* storage = m_componentTypes.Find<T>();
			return CanMutateStructure() && IsAlive(entity) && storage != nullptr && storage->Replace(entity, std::move(component));
		}

		template <ComponentStorageCompatible T> const T* Get(EntityId entity) const noexcept
		{
			const ComponentStorage<T>* storage = m_componentTypes.Find<T>();
			return IsAlive(entity) && storage != nullptr ? storage->Get(entity) : nullptr;
		}

		template <ComponentStorageCompatible T> const ComponentStorage<T>* FindStorage() const noexcept { return m_componentTypes.Find<T>(); }

	private:
		friend class StructureFrozenEpoch;
		template <typename... AccessSpecs> friend class Query;

		struct EntitySlot final
		{
			EntityId::Generation Generation = 1;
			bool Alive = false;
			bool Retired = false;
		};

		bool CanMutateStructure() const noexcept { return !m_structureFrozen; }

		bool IsFrozenEpochCurrent(std::uint64_t generation) const noexcept { return m_structureFrozen && generation != 0 && m_frozenEpochGeneration == generation; }

		void ReleaseFrozenEpoch(std::uint64_t generation) noexcept;

		void AdvanceStructureVersion() noexcept { ++m_structureVersion; }

		template <ComponentStorageCompatible T> ComponentStorage<T>* FindStorageForQuery() noexcept { return m_componentTypes.Find<T>(); }

		std::vector<EntitySlot> m_slots;
		std::vector<EntityId::Slot> m_freeSlots;
		ComponentTypeRegistry m_componentTypes;
		std::size_t m_liveCount = 0;
		std::uint64_t m_structureVersion = 0;
		std::uint64_t m_frozenEpochGeneration = 0;
		bool m_structureFrozen = false;
	};
}
