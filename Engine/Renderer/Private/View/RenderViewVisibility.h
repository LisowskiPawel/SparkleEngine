#pragma once

#include "View/MeshInstanceBatchBuilder.h"

#include <vector>

struct PreparedRenderScene;
struct RenderView;

struct RenderViewVisibilityRun final
{
	const PreparedRenderScene* Scene = nullptr;
	const RenderView* View = nullptr;
	std::vector<MeshRenderItem> Items;
};

void EvaluateRenderViewVisibility(RenderViewVisibilityRun& run, std::uint32_t begin, std::uint32_t end) noexcept;
void BuildRenderViewWorkload(const PreparedRenderScene& scene, RenderView& view) noexcept;
