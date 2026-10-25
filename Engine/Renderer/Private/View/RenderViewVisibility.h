#pragma once

#include "Tasks/Public/TaskTypes.h"
#include "View/MeshInstanceBatchBuilder.h"

#include <vector>

class TaskExecutionContext;
struct PreparedRenderScene;
struct RenderView;

struct RenderViewVisibilityRun final
{
	const PreparedRenderScene* Scene = nullptr;
	const RenderView* View = nullptr;
	std::vector<MeshRenderItem> Items;
};

TaskResult EvaluateRenderViewVisibility(std::uint32_t begin, std::uint32_t end, TaskExecutionContext& context);
void BuildRenderViewWorkload(const PreparedRenderScene& scene, RenderView& view) noexcept;
