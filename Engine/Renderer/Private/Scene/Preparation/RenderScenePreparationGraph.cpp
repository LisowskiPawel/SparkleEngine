#include "PCH.h"
#include "Scene/Preparation/RenderScenePreparationGraph.h"

#include "Scene/Preparation/RenderScenePreparationRun.h"
#include "Scene/Preparation/RenderScenePreparationMerger.h"
#include "Scene/Preparation/RenderScenePreparationTasks.h"
#include "Tasks/Public/ParallelFor.h"

#include <algorithm>
#include <array>
#include <bit>
#include <limits>
#include <string_view>

static constexpr std::uint32_t PrimitiveSerialThreshold = 128u;
static constexpr std::uint32_t LightingSerialThreshold = 32u;
static constexpr std::uint32_t DeformationSerialThreshold = 64u;

static std::uint32_t ResolveCapacity(std::size_t count, std::uint32_t serialThreshold) noexcept
{
	const std::size_t maximumCapacity = (std::numeric_limits<std::uint32_t>::max)();
	const std::uint32_t boundedCount = static_cast<std::uint32_t>((std::min) (count, maximumCapacity));

	return boundedCount <= serialThreshold ? serialThreshold : std::bit_ceil(boundedCount);
}

RenderScenePreparationCapacity ResolveRenderScenePreparationCapacity(const RenderScenePreparationRun& run) noexcept
{
	return RenderScenePreparationCapacity{
	    .Primitives = ResolveCapacity(run.ResolvedPrimitives.size(), PrimitiveSerialThreshold),
	    .Lights = ResolveCapacity(run.PreparedLights.size(), LightingSerialThreshold),
	    .JointMatrixCopyRanges = ResolveCapacity(run.Deformation.JointMatrixCopyRanges.size(), DeformationSerialThreshold),
	    .MorphWeightCopyRanges = ResolveCapacity(run.Deformation.MorphWeightCopyRanges.size(), DeformationSerialThreshold)};
}

static TaskDesc MakeTaskDesc(std::string_view name)
{
	return TaskDesc{.Name = TaskName(name), .Lane = TaskLane::FrameCritical};
}

static TaskNodeHandle AddTransformBoundsTasks(TaskGraphBuilder& builder, std::uint32_t capacity)
{
	return ParallelFor(
	    builder,
	    MakeTaskDesc("Renderer.Scene.Preparation.TransformBounds"),
	    capacity,
	    ParallelForPolicy{.GrainSize = 64u, .SerialThreshold = PrimitiveSerialThreshold, .MaximumPartitions = 8u},
	    &RenderScenePreparationTasks::TransformPrimitives);
}

static TaskNodeHandle AddJointMatrixTasks(TaskGraphBuilder& builder, std::uint32_t capacity)
{
	return ParallelFor(
	    builder,
	    MakeTaskDesc("Renderer.Scene.Preparation.JointMatrices"),
	    capacity,
	    ParallelForPolicy{.GrainSize = 16u, .SerialThreshold = DeformationSerialThreshold, .MaximumPartitions = 8u},
	    &RenderScenePreparationTasks::CopyJointMatrices);
}

static TaskNodeHandle AddMorphWeightTasks(TaskGraphBuilder& builder, std::uint32_t capacity)
{
	return ParallelFor(
	    builder,
	    MakeTaskDesc("Renderer.Scene.Preparation.MorphWeights"),
	    capacity,
	    ParallelForPolicy{.GrainSize = 16u, .SerialThreshold = DeformationSerialThreshold, .MaximumPartitions = 8u},
	    &RenderScenePreparationTasks::CopyMorphWeights);
}

static TaskNodeHandle AddLightingTasks(TaskGraphBuilder& builder, std::uint32_t capacity)
{
	return ParallelFor(
	    builder,
	    MakeTaskDesc("Renderer.Scene.Preparation.Lighting"),
	    capacity,
	    ParallelForPolicy{.GrainSize = 16u, .SerialThreshold = LightingSerialThreshold, .MaximumPartitions = 4u},
	    &RenderScenePreparationTasks::PrepareLights);
}

static void AddMergeTasks(TaskGraphBuilder& builder, TaskNodeHandle primitives, TaskNodeHandle jointMatrices, TaskNodeHandle morphWeights, TaskNodeHandle lighting)
{
	const std::array<TaskNodeHandle, 4> mergeInputs{primitives, jointMatrices, morphWeights, lighting};
	const TaskNodeHandle mergeJoin = builder.WhenAll(MakeTaskDesc("Renderer.Scene.Preparation.MergeJoin"), mergeInputs);
	const TaskNodeHandle merge = builder.ContinueWith(mergeJoin, MakeTaskDesc("Renderer.Scene.Preparation.Merge"), &RenderScenePreparationMerger::Merge);

	(void) builder.ContinueWith(merge, MakeTaskDesc("Renderer.Scene.Preparation.RayTracingPlan"), &RenderScenePreparationMerger::BuildRayTracingPlan);
}

CompiledTaskGraph BuildRenderScenePreparationGraph(const RenderScenePreparationCapacity& capacity)
{
	TaskGraphBuilder builder;

	const TaskNodeHandle primitives = AddTransformBoundsTasks(builder, capacity.Primitives);
	const TaskNodeHandle jointMatrices = AddJointMatrixTasks(builder, capacity.JointMatrixCopyRanges);
	const TaskNodeHandle morphWeights = AddMorphWeightTasks(builder, capacity.MorphWeightCopyRanges);
	const TaskNodeHandle lighting = AddLightingTasks(builder, capacity.Lights);

	AddMergeTasks(builder, primitives, jointMatrices, morphWeights, lighting);
	return builder.Compile();
}
