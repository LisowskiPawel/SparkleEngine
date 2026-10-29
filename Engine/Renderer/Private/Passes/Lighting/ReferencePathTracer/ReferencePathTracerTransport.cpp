#include "PCH.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerTransport.h"

#include "Frame/RenderFrame.h"
#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerResources.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerSession.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerShader.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerWork.h"
#include "RayTracing/RayTracingMaterialPass.h"
#include "Scene/RayTracing/RenderRayTracingScene.h"
#include "ShaderData/SceneShaderParameters.h"

template <typename TShader> static auto& BuildReferencePathTracerParameters(
    FrameGraphBuilder& builder,
    const RenderFrame& frame,
    const RenderFrameGraphResources& resources,
    const ReferencePathTracerGraphResources& graphResources,
    const ReferencePathTracerWork& work)
{
	auto& parameters = builder.AllocParameters<TShader>();
	parameters->WorkingMean = builder.CreateUAV(graphResources.WorkingMean);
	parameters->WorkingM2 = builder.CreateUAV(graphResources.WorkingM2);
	parameters->CommittedMean = builder.CreateSRV(graphResources.CommittedMean);
	parameters->CommittedM2 = builder.CreateSRV(graphResources.CommittedM2);

	BindSceneShaderParameters(builder, frame, parameters, resources);
	parameters->SessionSeed = work.SessionSeed;
	parameters->ReplicateId = work.ReplicateId;
	parameters->SampleOrdinal = work.SampleOrdinal;
	parameters->FinitePathDiagnosticSurfaceVertices = work.FinitePathDiagnosticSurfaceVertices;
	parameters->FirstRow = work.FirstRow;
	parameters->RowCount = work.RowCount;
	parameters->PriorSampleCount = work.PriorSampleCount;
	parameters->WorkFlags = work.WorkFlags;

	return parameters;
}

void AddReferencePathTracerTransportPass(
    FrameGraphBuilder& builder,
    const RenderFrame& frame,
    RenderViewportExtent extent,
    const ReferencePathTracerGraphResources& graphResources,
    const ReferencePathTracerWork& work,
    RenderRayTracingScene& rayTracingScene,
    const RenderFrameGraphResources& resources)
{
	AddRayTracingMaterialPass<ReferencePathTracerInlineCS, ReferencePathTracerRGS>(
	    builder,
	    "ReferencePathTracer.SurfaceTransportReference",
	    rayTracingScene,
	    ComputeDispatchDesc{MathUtils::DivideRoundUp(extent.Width, 8u), MathUtils::DivideRoundUp(ReferencePathTracerSession::WorkRowsPerDispatch, 8u), 1u},
	    RayTracingDispatchDimensions{.Width = extent.Width, .Height = ReferencePathTracerSession::WorkRowsPerDispatch, .Depth = 1u},
	    [&]<typename TShader>() -> auto& { return BuildReferencePathTracerParameters<TShader>(builder, frame, resources, graphResources, work); });
}
