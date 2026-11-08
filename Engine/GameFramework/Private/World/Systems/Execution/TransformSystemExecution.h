#pragma once

#include "GameFramework/Public/World/EntityId.h"

#include <cstdint>
#include <vector>

namespace ECS
{
	class EntityRegistry;

	class TransformSystemExecution final
	{
	public:
		TransformSystemExecution(EntityRegistry& registry, const std::vector<EntityId>& dirtyTransforms);

		std::uint32_t GetDirtyTransformCount() const noexcept;
		bool RunTransforms(std::uint32_t begin, std::uint32_t end);
		bool RunCameraDerivedState(std::uint32_t begin, std::uint32_t end);

	private:
		EntityRegistry& m_registry;
		const std::vector<EntityId>& m_dirtyTransforms;
	};
}
