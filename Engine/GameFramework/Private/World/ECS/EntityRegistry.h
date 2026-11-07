#pragma once

#include "GameFramework/Public/World/EntityId.h"
#include "World/ECS/WorldComponentStores.h"
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
		EntityRegistry() = default;
		EntityRegistry(const EntityRegistry&) = delete;
		EntityRegistry& operator=(const EntityRegistry&) = delete;
		EntityRegistry(EntityRegistry&&) = delete;
		EntityRegistry& operator=(EntityRegistry&&) = delete;

		EntityId Create();
		bool Destroy(EntityId entity);
		bool IsAlive(EntityId entity) const noexcept;

		std::size_t GetLiveCount() const noexcept { return m_liveCount; }

		std::uint64_t GetStructureVersion() const noexcept { return m_structureVersion; }

		bool IsStructureFrozen() const noexcept { return m_bStructureFrozen; }

		StructureFrozenEpoch FreezeStructure() noexcept;

		template <ComponentStorageCompatible T> bool Add(EntityId entity, T component)
		{
			if (!CanMutateStructure() || !IsAlive(entity))
			{
				return false;
			}
			const bool bAdded = std::get<ComponentStorage<T>>(m_components).Add(entity, std::move(component));
			if (bAdded)
			{
				AdvanceStructureVersion();
			}
			return bAdded;
		}

		template <ComponentStorageCompatible T> bool Replace(EntityId entity, T component)
		{
			return CanMutateStructure() && IsAlive(entity) && std::get<ComponentStorage<T>>(m_components).Replace(entity, std::move(component));
		}

		template <ComponentStorageCompatible T> const T* Get(EntityId entity) const noexcept { return IsAlive(entity) ? GetStorage<T>().Get(entity) : nullptr; }

		template <ComponentStorageCompatible T> const ComponentStorage<T>& GetStorage() const noexcept { return std::get<ComponentStorage<T>>(m_components); }

	private:
		friend class StructureFrozenEpoch;
		template <typename... AccessSpecs> friend class Query;

		struct EntitySlot final
		{
			EntityId::Generation Generation = 1;
			bool bAlive = false;
			bool bRetired = false;
		};

		bool CanMutateStructure() const noexcept { return !m_bStructureFrozen; }

		bool IsFrozenEpochCurrent(std::uint64_t generation) const noexcept { return m_bStructureFrozen && generation != 0 && m_frozenEpochGeneration == generation; }

		void ReleaseFrozenEpoch(std::uint64_t generation) noexcept;

		void AdvanceStructureVersion() noexcept { ++m_structureVersion; }

		template <ComponentStorageCompatible T> ComponentStorage<T>& GetStorageForQuery() noexcept { return std::get<ComponentStorage<T>>(m_components); }

		std::vector<EntitySlot> m_slots;
		std::vector<EntityId::Slot> m_freeSlots;
		WorldComponentStores m_components;
		std::size_t m_liveCount = 0;
		std::uint64_t m_structureVersion = 0;
		std::uint64_t m_frozenEpochGeneration = 0;
		bool m_bStructureFrozen = false;
	};
}
