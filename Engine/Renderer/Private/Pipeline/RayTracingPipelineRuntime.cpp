#include "PCH.h"

#include "Pipeline/RayTracingPipelineRuntime.h"

#include "Core/Public/Diagnostics/Error.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"

std::unique_ptr<RayTracingPipelineRuntime> RayTracingPipelineRuntime::Create(
    RenderHardwareInterface& renderHardwareInterface,
    const GlobalShaderMap& map,
    const CookedShaderLibrary& library,
    ShaderTarget target,
    std::uint64_t generation,
    const RayTracingPipelineComposition& composition)
{
	if (!renderHardwareInterface.GetCapabilities().RayTracing.bSupportsRayTracingPipeline || generation == 0)
	{
		throw Diagnostics::Error("Ray-tracing pipeline materialization requires complete backend readiness and a generation.");
	}
	auto runtime = std::make_unique<RayTracingPipelineRuntime>();
	runtime->m_generation = generation;

	const auto exports = runtime->ResolveShaders(renderHardwareInterface, map, library, target, composition);
	runtime->MaterializePipeline(renderHardwareInterface, composition, exports);
	return runtime;
}
