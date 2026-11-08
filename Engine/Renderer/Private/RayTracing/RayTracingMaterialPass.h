#pragma once

#include "Core/Public/Diagnostics/Error.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "RayTracing/RayTracingExecutionFrontend.h"
#include "RayTracing/RayTracingMaterialPipelineShaders.h"
#include "RayTracing/RayTracingPipelineComposition.h"
#include "Scene/RayTracing/RenderRayTracingScene.h"

#include <string_view>
#include <vector>

std::vector<const RayTracingHitGroupComposition*> ResolveRayTracingMaterialRecordGroups(const RayTracingPipelineComposition& composition, const RayTracingShaderTablePlan& plan);

template <typename TInlineShader, typename TBuildParameters>
void AddInlineRayTracingMaterialPass(FrameGraphBuilder& builder, std::string_view label, const ComputeDispatchDesc& dimensions, TBuildParameters&& buildParameters)
{
	auto& parameters = buildParameters.template operator()<TInlineShader>();
	builder.Dispatch<TInlineShader>(label, parameters, dimensions);
}

template <typename TRayGenerationShader, typename TBuildParameters> void AddPipelineRayTracingMaterialPass(
    FrameGraphBuilder& builder,
    std::string_view label,
    RayTracingShaderTablePlan& plan,
    const RayTracingDispatchDimensions& dimensions,
    TBuildParameters&& buildParameters)
{
	static const RayTracingPipelineComposition composition = RayTracingPipelineComposition::Create<TRayGenerationShader>(
	    std::vector{RayTracingPipelineComposition::Shader<RayTracingMaterialMiss>()},
	    std::vector{
	        RayTracingHitGroupComposition::Triangles<RayTracingMaterialClosestHit>("RayTracingMaterialOpaqueHitGroup"),
	        RayTracingHitGroupComposition::Triangles<RayTracingMaterialClosestHit, RayTracingMaterialAnyHit>("RayTracingMaterialAlphaTestedHitGroup")});

	auto& parameters = buildParameters.template operator()<TRayGenerationShader>();

	const auto materialization = builder.TraceRays<TRayGenerationShader>(
	    label,
	    composition,
	    plan.GetGeneration(),
	    parameters,
	    dimensions,
	    [&plan]() { return ResolveRayTracingMaterialRecordGroups(composition, plan); });

	if (materialization)
	{
		plan.RecordMaterialization(materialization->SizeInBytes, materialization->ElapsedMicroseconds);
	}
}

template <typename TInlineShader, typename TRayGenerationShader, typename TBuildParameters> void AddRayTracingMaterialPass(
    FrameGraphBuilder& builder,
    std::string_view label,
    RenderRayTracingScene& scene,
    ComputeDispatchDesc inlineDimensions,
    RayTracingDispatchDimensions pipelineDimensions,
    TBuildParameters&& buildParameters)
{
	switch (scene.GetExecutionFrontend())
	{
		case RayTracingExecutionFrontend::Inline:
			AddInlineRayTracingMaterialPass<TInlineShader>(builder, label, inlineDimensions, std::forward<TBuildParameters>(buildParameters));
			return;
		case RayTracingExecutionFrontend::Pipeline:
			AddPipelineRayTracingMaterialPass<TRayGenerationShader>(builder, label, scene.GetShaderTablePlan(), pipelineDimensions, std::forward<TBuildParameters>(buildParameters));
			return;
		case RayTracingExecutionFrontend::None:
		default:
			throw Diagnostics::Error("Ray-tracing material pass has no supported execution frontend.");
	}
}
