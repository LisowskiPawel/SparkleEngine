#include "PCH.h"

#include "Scene/Preparation/RenderScenePreparationMerger.h"

#include "Scene/Preparation/RenderLightPreparation.h"
#include "Scene/Preparation/RenderScenePreparationRun.h"
#include "Tasks/Public/TaskExecutionContext.h"

#include <utility>

TaskResult RenderScenePreparationMerger::Merge(TaskExecutionContext& context)
{
	RenderScenePreparationRun& run = *context.TryGet<RenderScenePreparationRun>();

	CommitPreparedRenderLights(run.PreparedLights, run.PreparedScene);
	run.PreparedScene.primitives = std::move(run.PreparedPrimitives);
	return TaskResult::Success();
}

void RenderScenePreparationMerger::PublishSceneOutputs(RenderScenePreparationRun& run)
{
	run.PreparedScene.jointMatrices = std::move(run.Deformation.JointMatrices);
	run.PreparedScene.previousJointMatrices = std::move(run.Deformation.PreviousJointMatrices);
	run.PreparedScene.morphWeights = std::move(run.Deformation.MorphWeights);
	run.PreparedScene.previousMorphWeights = std::move(run.Deformation.PreviousMorphWeights);
}

TaskResult RenderScenePreparationMerger::BuildRayTracingPlan(TaskExecutionContext& context)
{
	RenderScenePreparationRun& run = *context.TryGet<RenderScenePreparationRun>();
	auto& rayTracingInstances = run.PreparedScene.rayTracingInstances;

	rayTracingInstances.reserve(run.PreparedScene.primitives.size());

	for (std::uint32_t primitiveIndex = 0u; primitiveIndex < run.PreparedScene.primitives.size(); ++primitiveIndex)
	{
		const MeshDraw& draw = run.PreparedScene.primitives[primitiveIndex].Draw;
		if (!draw.Geometry.Mesh)
		{
			continue;
		}

		rayTracingInstances.push_back(RenderRayTracingInstance{.PrimitiveIndex = primitiveIndex, .GpuSceneSlot = draw.Source.GpuSceneSlot});
	}

	return TaskResult::Success();
}
