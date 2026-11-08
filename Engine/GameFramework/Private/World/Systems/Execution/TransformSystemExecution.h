#pragma once

#include "World/Systems/Descriptors/GameWorldSystemContract.h"

#include <cstdint>
#include <span>
#include <vector>

namespace ECS
{
	class TransformSystemExecution final
	{
	public:
		TransformSystemExecution(
		    TransformEvaluationQuery& transforms,
		    CameraDerivedStateQuery& cameras,
		    const std::vector<EntityId>& dirtyTransforms,
		    std::span<EntityId> evaluated,
		    std::span<EntityId> cameraChanges);

		std::uint32_t GetDirtyTransformCount() const noexcept;
		bool RunTransforms(std::uint32_t begin, std::uint32_t end);
		bool RunCameraDerivedState(std::uint32_t begin, std::uint32_t end);

	private:
		TransformEvaluationQuery& m_transformQuery;
		CameraDerivedStateQuery& m_cameraDerivedQuery;
		const std::vector<EntityId>& m_dirtyTransforms;
		std::span<EntityId> m_evaluated;
		std::span<EntityId> m_cameraChanges;
	};
}
