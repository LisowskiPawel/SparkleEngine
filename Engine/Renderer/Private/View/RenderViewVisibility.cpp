#include "PCH.h"
#include "View/RenderViewVisibility.h"

#include "Core/Public/Math/Frustum.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "Tasks/Public/TaskExecutionContext.h"
#include "View/RenderView.h"

#include <algorithm>

static bool Intersects(const Frustum& frustum, const RenderMeshWorldBounds& bounds) noexcept
{
	if (!bounds.Valid)
	{
		return true;
	}

	for (const DirectX::XMFLOAT4& plane : frustum.planes)
	{
		const DirectX::XMFLOAT3 positive{plane.x >= 0.0f ? bounds.Max.x : bounds.Min.x, plane.y >= 0.0f ? bounds.Max.y : bounds.Min.y, plane.z >= 0.0f ? bounds.Max.z : bounds.Min.z};
		if (plane.x * positive.x + plane.y * positive.y + plane.z * positive.z + plane.w < 0.0f)
		{
			return false;
		}
	}
	return true;
}

static RenderMaterialClassification ClassifyMaterial(std::uint32_t alphaMode) noexcept
{
	switch (alphaMode)
	{
		case 0u:
			return RenderMaterialClassification::Opaque;
		case 1u:
			return RenderMaterialClassification::AlphaTested;
		case 2u:
			return RenderMaterialClassification::Transparent;
		default:
			return RenderMaterialClassification::Rejected;
	}
}

static float ComputeCameraDistanceSquared(const DirectX::XMFLOAT3& cameraPosition, const RenderMeshWorldBounds& bounds, const DirectX::XMFLOAT4X4& worldMatrix) noexcept
{
	const DirectX::XMFLOAT3 center = bounds.Valid ? DirectX::XMFLOAT3{0.5f * (bounds.Min.x + bounds.Max.x), 0.5f * (bounds.Min.y + bounds.Max.y), 0.5f * (bounds.Min.z + bounds.Max.z)}
	                                              : DirectX::XMFLOAT3{worldMatrix._41, worldMatrix._42, worldMatrix._43};

	const float x = center.x - cameraPosition.x;
	const float y = center.y - cameraPosition.y;
	const float z = center.z - cameraPosition.z;
	return x * x + y * y + z * z;
}

TaskResult EvaluateRenderViewVisibility(std::uint32_t begin, std::uint32_t end, TaskExecutionContext& context)
{
	RenderViewVisibilityRun& run = *context.TryGet<RenderViewVisibilityRun>();
	const std::size_t rangeBegin = (std::min<std::size_t>) (begin, run.Scene->primitives.size());
	const std::size_t rangeEnd = (std::min<std::size_t>) (end, run.Scene->primitives.size());
	for (std::size_t primitiveIndex = rangeBegin; primitiveIndex < rangeEnd; ++primitiveIndex)
	{
		const PreparedRenderPrimitive& primitive = run.Scene->primitives[primitiveIndex];
		const RenderMaterialClassification classification = ClassifyMaterial(primitive.MaterialAlphaMode);
		const bool visible = classification != RenderMaterialClassification::Rejected && Intersects(run.View->frustum, primitive.WorldBounds);

		run.Items[primitiveIndex] = visible
		    ? MeshRenderItem{
		          .Object = primitive.Object,
		          .DrawIndex = static_cast<std::uint32_t>(primitiveIndex),
		          .Material = primitive.Material,
		          .InstanceGroupIndex = primitive.InstanceGroupIndex,
		          .Classification = classification,
		          .CameraDistanceSquared = ComputeCameraDistanceSquared(
		              run.View->cameraUniform.Position,
		              primitive.WorldBounds,
		              primitive.Draw.Transform.WorldMatrix)}
		    : MeshRenderItem{};
	}
	return TaskResult::Success();
}

void BuildRenderViewWorkload(const PreparedRenderScene& scene, RenderView& view) noexcept
{
	view.workload = {};
	view.workload.jointMatrixCount = static_cast<std::uint32_t>(scene.jointMatrices.size());
	for (std::uint32_t primitiveIndex : view.rasterPrimitiveIndices)
	{
		if (primitiveIndex >= scene.primitives.size())
		{
			continue;
		}
		scene.primitives[primitiveIndex].Draw.Geometry.MeshKind == RenderMeshKind::Skeletal ? ++view.workload.skinnedInstanceCount : ++view.workload.staticInstanceCount;
	}
	for (const MeshInstanceBatch& batch : view.meshInstanceBatches)
	{
		batch.meshKind == RenderMeshKind::Skeletal ? ++view.workload.skinnedBatchCount : ++view.workload.staticBatchCount;
	}
}
