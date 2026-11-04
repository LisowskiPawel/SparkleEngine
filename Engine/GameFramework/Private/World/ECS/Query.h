#pragma once

#include "World/ECS/EntityRegistry.h"
#include "World/ECS/QueryAccess.h"

#include <cstdint>
#include <array>
#include <cstddef>
#include <functional>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ECS
{
	template <typename... AccessSpecs> class Query final
	{
		static_assert((QueryAccessSpec<AccessSpecs> && ...), "Query arguments must be Read<T> or Write<T>.");
		static_assert(sizeof...(AccessSpecs) > 0, "A query requires at least one component access.");
		static_assert(HaveUniqueQueryComponents<AccessSpecs...>(), "A component may appear only once in a query.");

		using StorageTuple = std::tuple<ComponentStorage<typename QueryAccessTraits<AccessSpecs>::Component>*...>;

	public:
		Query(EntityRegistry& registry, const StructureFrozenEpoch& epoch) noexcept :
		    m_registry(&registry),
		    m_epochGeneration(epoch.m_generation),
		    m_registryStructureVersion(registry.GetStructureVersion())
		{
			if (epoch.m_registry != &registry || !registry.IsFrozenEpochCurrent(m_epochGeneration))
			{
				m_registry = nullptr;
				return;
			}

			m_storages = StorageTuple{&registry.GetStorageForQuery<typename QueryAccessTraits<AccessSpecs>::Component>()...};
			InitializePlan(std::index_sequence_for<AccessSpecs...>{});
		}

		Query(const Query&) = delete;
		Query& operator=(const Query&) = delete;
		Query(Query&&) = delete;
		Query& operator=(Query&&) = delete;

		static constexpr std::array<ComponentAccessDesc, sizeof...(AccessSpecs)> GetAccessMetadata() noexcept
		{
			return {ComponentAccessDesc{GetWorldComponentIndex<typename QueryAccessTraits<AccessSpecs>::Component>(), QueryAccessTraits<AccessSpecs>::Mode}...};
		}

		bool IsValid() const noexcept { return GetValidity() == QueryIterationStatus::Success; }

		std::size_t GetEstimatedEntityCount() const noexcept { return m_leadingEntities.size(); }

		bool PrepareWriteTraversal() noexcept
		{
			if (GetValidity() != QueryIterationStatus::Success)
			{
				return false;
			}
			MarkWrites(std::index_sequence_for<AccessSpecs...>{});
			return true;
		}

		template <typename Function> QueryIterationResult ForEachRange(std::size_t begin, std::size_t end, Function&& function) const
		{
			const QueryIterationStatus initialStatus = GetValidity();
			if (initialStatus != QueryIterationStatus::Success)
			{
				return {.Status = initialStatus};
			}
			if (begin > end || end > m_leadingEntities.size())
			{
				return {.Status = QueryIterationStatus::StaleView};
			}

			std::size_t entityCount = 0;
			for (std::size_t leadingIndex = begin; leadingIndex < end; ++leadingIndex)
			{
				const EntityId entity = m_leadingEntities[leadingIndex];
				if (!Matches(entity, std::index_sequence_for<AccessSpecs...>{}))
				{
					continue;
				}
				auto arguments = BuildArguments(entity, std::index_sequence_for<AccessSpecs...>{});
				std::apply([&](auto&... components) { std::invoke(function, leadingIndex, entity, components...); }, arguments);
				++entityCount;
			}
			return {.Status = GetValidity(), .EntityCount = entityCount};
		}

		template <typename Function> QueryIterationResult ForEachEntityRange(std::span<const EntityId> entities, std::size_t begin, std::size_t end, Function&& function) const
		{
			const QueryIterationStatus initialStatus = GetValidity();
			if (initialStatus != QueryIterationStatus::Success)
			{
				return {.Status = initialStatus};
			}
			if (begin > end || end > entities.size())
			{
				return {.Status = QueryIterationStatus::StaleView};
			}

			std::size_t entityCount = 0;
			for (std::size_t targetIndex = begin; targetIndex < end; ++targetIndex)
			{
				const EntityId entity = entities[targetIndex];
				if (!Matches(entity, std::index_sequence_for<AccessSpecs...>{}))
				{
					continue;
				}
				auto arguments = BuildArguments(entity, std::index_sequence_for<AccessSpecs...>{});
				std::apply([&](auto&... components) { std::invoke(function, targetIndex, entity, components...); }, arguments);
				++entityCount;
			}
			return {.Status = GetValidity(), .EntityCount = entityCount};
		}

	private:
		template <typename AccessSpec> void ConsiderLeadingStorage(ComponentStorage<typename QueryAccessTraits<AccessSpec>::Component>* storage) noexcept
		{
			const std::span<const EntityId> entities = storage->GetEntities();
			if (entities.size() < m_leadingEntities.size())
			{
				m_leadingEntities = entities;
			}
		}

		template <std::size_t... Indices> void InitializePlan(std::index_sequence<Indices...>) noexcept
		{
			m_leadingEntities = std::get<0>(m_storages)->GetEntities();
			(ConsiderLeadingStorage<AccessSpecs>(std::get<Indices>(m_storages)), ...);
			m_storageStructureVersions = {std::get<Indices>(m_storages)->GetVersion().Structure...};
		}

		template <std::size_t... Indices> bool Matches(EntityId entity, std::index_sequence<Indices...>) const noexcept { return (std::get<Indices>(m_storages)->Contains(entity) && ...); }

		template <typename AccessSpec> auto BuildArgument(EntityId entity, ComponentStorage<typename QueryAccessTraits<AccessSpec>::Component>* storage) const
		{
			using Component = typename QueryAccessTraits<AccessSpec>::Component;
			if constexpr (QueryAccessTraits<AccessSpec>::Mode == ComponentAccessMode::Read)
			{
				return std::tuple<const Component&>(*storage->Get(entity));
			}
			else
			{
				return std::tuple<Component&>(*storage->GetMutable(entity));
			}
		}

		template <std::size_t... Indices> auto BuildArguments(EntityId entity, std::index_sequence<Indices...>) const
		{
			return std::tuple_cat(BuildArgument<AccessSpecs>(entity, std::get<Indices>(m_storages))...);
		}

		template <typename AccessSpec> void MarkWrite(ComponentStorage<typename QueryAccessTraits<AccessSpec>::Component>* storage) noexcept
		{
			if constexpr (QueryAccessTraits<AccessSpec>::Writable)
			{
				storage->MarkContentChanged();
			}
		}

		template <std::size_t... Indices> void MarkWrites(std::index_sequence<Indices...>) noexcept { (MarkWrite<AccessSpecs>(std::get<Indices>(m_storages)), ...); }

		QueryIterationStatus GetValidity() const noexcept
		{
			if (m_registry == nullptr || !m_registry->IsFrozenEpochCurrent(m_epochGeneration))
			{
				return QueryIterationStatus::InvalidEpoch;
			}
			if (m_registry->GetStructureVersion() != m_registryStructureVersion || !StorageVersionsMatch(std::index_sequence_for<AccessSpecs...>{}))
			{
				return QueryIterationStatus::StaleView;
			}
			return QueryIterationStatus::Success;
		}

		template <std::size_t... Indices> bool StorageVersionsMatch(std::index_sequence<Indices...>) const noexcept
		{
			return ((std::get<Indices>(m_storages)->GetVersion().Structure == m_storageStructureVersions[Indices]) && ...);
		}

		EntityRegistry* m_registry = nullptr;
		std::uint64_t m_epochGeneration = 0;
		std::uint64_t m_registryStructureVersion = 0;
		StorageTuple m_storages;
		std::array<std::uint64_t, sizeof...(AccessSpecs)> m_storageStructureVersions{};
		std::span<const EntityId> m_leadingEntities;
	};
}
