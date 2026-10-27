#include "PCH.h"

#include "Pipeline/RayTracingPipelineRuntime.h"

#include "Core/Public/Diagnostics/Error.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "Scene/RayTracing/RayTracingShaderTablePlan.h"

std::unique_ptr<RayTracingPipelineRuntime> RayTracingPipelineRuntime::Create(
    RenderHardwareInterface& renderHardwareInterface,
    const GlobalShaderMap& map,
    const CookedShaderLibrary& library,
    ShaderTarget target,
    std::uint64_t generation,
    const RayTracingPipelineComposition& composition)
{
	if (!renderHardwareInterface.GetCapabilities().RayTracing.SupportsRayTracingPipeline || generation == 0)
	{
		throw Diagnostics::Error("Ray-tracing pipeline materialization requires complete backend readiness and a generation.");
	}
	auto runtime = std::make_unique<RayTracingPipelineRuntime>();
	runtime->m_generation = generation;

	const auto exports = runtime->ResolveShaders(renderHardwareInterface, map, library, target, composition);
	runtime->MaterializePipeline(renderHardwareInterface, composition, exports);
	return runtime;
}

std::unique_ptr<RayTracingShaderTable> RayTracingPipelineRuntime::CreateShaderTable(RenderHardwareInterface& renderHardwareInterface, const RayTracingPipelineComposition& composition) const
{
	const auto recordGroups = CollectRecordGroups(composition);
	return CreateShaderTable(renderHardwareInterface, composition, recordGroups);
}

std::unique_ptr<RayTracingShaderTable> RayTracingPipelineRuntime::CreateShaderTable(
    RenderHardwareInterface& renderHardwareInterface,
    const RayTracingPipelineComposition& composition,
    const RayTracingShaderTablePlan& plan) const
{
	if (m_pipeline == nullptr || composition.GetRayGeneration() == 0u || !plan.Validate())
	{
		throw Diagnostics::Error("Ray-tracing shader-table materialization received an invalid pipeline or scene plan.");
	}

	const auto recordGroups = ResolveSceneRecordGroups(composition, plan);
	return CreateShaderTable(renderHardwareInterface, composition, recordGroups);
}
