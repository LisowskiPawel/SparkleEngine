#pragma once

#include "Tasks/Public/TaskGraph.h"

#include <cstdint>

struct RenderScenePreparationRun;

struct RenderScenePreparationCapacity final
{
	std::uint32_t Primitives = 0u;
	std::uint32_t Lights = 0u;
	std::uint32_t JointMatrixCopyRanges = 0u;
	std::uint32_t MorphWeightCopyRanges = 0u;
	bool operator==(const RenderScenePreparationCapacity&) const = default;
};

RenderScenePreparationCapacity ResolveRenderScenePreparationCapacity(const RenderScenePreparationRun& run) noexcept;
CompiledTaskGraph BuildRenderScenePreparationGraph(const RenderScenePreparationCapacity& capacity);
