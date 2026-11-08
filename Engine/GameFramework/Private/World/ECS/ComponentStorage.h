#pragma once

#include "GameFramework/Public/World/EntityId.h"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

namespace ECS
{
	class EntityRegistry;

	template <typename T> concept ComponentStorageCompatible = std::copy_constructible<T> && std::is_copy_assignable_v<T> && std::is_nothrow_move_constructible_v<T>
	    && std::is_nothrow_move_assignable_v<T>;

	template <ComponentStorageCompatible T> class ComponentStorage final
	{
	public:
		ComponentStorage() = default;
		ComponentStorage(const ComponentStorage&) = delete;
		ComponentStorage& operator=(const ComponentStorage&) = delete;
		ComponentStorage(ComponentStorage&&) = delete;
		ComponentStorage& operator=(ComponentStorage&&) = delete;

		bool Add(EntityId entity, T component)
		{
			if (!entity.IsValid() || IsSlotOccupied(entity.GetSlot()) || m_entities.size() >= InvalidDenseIndex)
			{
				return false;
			}

			PrepareCapacity(m_entities.size() + 1, entity.GetSlot());
			const std::uint32_t denseIndex = static_cast<std::uint32_t>(m_entities.size());
			m_entities.push_back(entity);
			m_components.push_back(std::move(component));
			m_sparse[entity.GetSlot()] = denseIndex;
			return true;
		}

		void Remove(EntityId entity)
		{
			const std::uint32_t denseIndex = FindDenseIndex(entity);
			if (denseIndex == InvalidDenseIndex)
			{
				return;
			}

			const std::uint32_t lastIndex = static_cast<std::uint32_t>(m_entities.size() - 1);
			if (denseIndex != lastIndex)
			{
				m_entities[denseIndex] = m_entities[lastIndex];
				m_components[denseIndex] = std::move(m_components[lastIndex]);
				m_sparse[m_entities[denseIndex].GetSlot()] = denseIndex;
			}

			m_sparse[entity.GetSlot()] = InvalidDenseIndex;
			m_entities.pop_back();
			m_components.pop_back();
		}

		bool Replace(EntityId entity, T component)
		{
			const std::uint32_t denseIndex = FindDenseIndex(entity);
			if (denseIndex == InvalidDenseIndex)
			{
				return false;
			}
			m_components[denseIndex] = std::move(component);
			return true;
		}

		const T* Get(EntityId entity) const noexcept
		{
			const std::uint32_t denseIndex = FindDenseIndex(entity);
			return denseIndex == InvalidDenseIndex ? nullptr : &m_components[denseIndex];
		}

		std::span<const EntityId> GetEntities() const noexcept { return m_entities; }

		std::span<const T> GetComponents() const noexcept { return m_components; }

	private:
		friend class EntityRegistry;

		static constexpr std::uint32_t InvalidDenseIndex = (std::numeric_limits<std::uint32_t>::max)();

		std::uint32_t FindDenseIndex(EntityId entity) const noexcept
		{
			if (!entity.IsValid() || entity.GetSlot() >= m_sparse.size())
			{
				return InvalidDenseIndex;
			}
			const std::uint32_t denseIndex = m_sparse[entity.GetSlot()];
			return denseIndex < m_entities.size() && m_entities[denseIndex] == entity ? denseIndex : InvalidDenseIndex;
		}

		T* GetMutable(EntityId entity) noexcept
		{
			const std::uint32_t denseIndex = FindDenseIndex(entity);
			return denseIndex == InvalidDenseIndex ? nullptr : &m_components[denseIndex];
		}

		bool IsSlotOccupied(EntityId::Slot slot) const noexcept { return slot < m_sparse.size() && m_sparse[slot] < m_entities.size(); }

		void PrepareCapacity(std::size_t denseCapacity, EntityId::Slot maximumSlot)
		{
			const std::size_t capacity = (std::min) (m_entities.capacity(), m_components.capacity());
			if (denseCapacity > capacity)
			{
				const std::size_t growth = (std::max) (std::size_t{1}, capacity / 2);
				const std::size_t available = InvalidDenseIndex - capacity;
				const std::size_t nextCapacity = (std::max) (denseCapacity, capacity + (std::min) (growth, available));
				m_entities.reserve(nextCapacity);
				m_components.reserve(nextCapacity);
			}
			if (maximumSlot >= m_sparse.size())
			{
				m_sparse.resize(static_cast<std::size_t>(maximumSlot) + 1, InvalidDenseIndex);
			}
		}

		std::vector<std::uint32_t> m_sparse;
		std::vector<EntityId> m_entities;
		std::vector<T> m_components;
	};
}
