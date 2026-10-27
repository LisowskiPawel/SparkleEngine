#pragma once

#include "Pipeline/PassPipelineRuntime.h"
#include "RayTracing/RayTracingPipelineComposition.h"
#include "RHI/Public/RayTracing/RhiRayTracingPipelineDesc.h"
#include "RHI/Public/ShaderParameters/PassParameterLayout.h"

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

class CookedShaderLibrary;
class GlobalShaderMap;
class RenderHardwareInterface;
class RayTracingShaderTablePlan;

class RayTracingPipelineRuntime final
{
public:
	static std::unique_ptr<RayTracingPipelineRuntime> Create(
	    RenderHardwareInterface& renderHardwareInterface,
	    const GlobalShaderMap& map,
	    const CookedShaderLibrary& library,
	    ShaderTarget target,
	    std::uint64_t generation,
	    const RayTracingPipelineComposition& composition);

	RenderBindingLayout& GetBindingLayout() const noexcept { return *m_bindingLayout; }

	RayTracingPipeline& GetPipeline() const noexcept { return *m_pipeline; }

	std::uint64_t GetGeneration() const noexcept { return m_generation; }

	std::unique_ptr<RayTracingShaderTable> CreateShaderTable(RenderHardwareInterface& renderHardwareInterface, const RayTracingPipelineComposition& composition) const;
	std::unique_ptr<RayTracingShaderTable> CreateShaderTable(
	    RenderHardwareInterface& renderHardwareInterface,
	    const RayTracingPipelineComposition& composition,
	    const RayTracingShaderTablePlan& plan) const;

private:
	static const ShaderRegistrationDesc& GetRegistration(ShaderTypeId shaderType);
	std::vector<RhiRayTracingShaderExportDesc> ResolveShaders(
	    RenderHardwareInterface& renderHardwareInterface,
	    const GlobalShaderMap& map,
	    const CookedShaderLibrary& library,
	    ShaderTarget target,
	    const RayTracingPipelineComposition& composition);
	void MaterializePipeline(RenderHardwareInterface& renderHardwareInterface, const RayTracingPipelineComposition& composition, std::span<const RhiRayTracingShaderExportDesc> exports);
	static std::vector<RhiRayTracingHitGroupDesc> BuildHitGroups(const RayTracingPipelineComposition& composition);
	static std::vector<const RayTracingHitGroupComposition*> CollectRecordGroups(const RayTracingPipelineComposition& composition);
	static std::vector<const RayTracingHitGroupComposition*> ResolveSceneRecordGroups(const RayTracingPipelineComposition& composition, const RayTracingShaderTablePlan& plan);
	static std::vector<RhiRayTracingShaderRecord> BuildShaderRecords(std::span<const ShaderTypeId> shaderTypes);
	static std::vector<RhiRayTracingShaderRecord> BuildHitGroupRecords(std::span<const RayTracingHitGroupComposition* const> recordGroups);
	std::unique_ptr<RayTracingShaderTable> CreateShaderTable(
	    RenderHardwareInterface& renderHardwareInterface,
	    const RayTracingPipelineComposition& composition,
	    std::span<const RayTracingHitGroupComposition* const> recordGroups) const;

	std::uint64_t m_generation = 0;
	PassParameterLayout m_parameterLayout;
	std::vector<ResolvedShader> m_shaders;
	std::unique_ptr<RenderBindingLayout> m_bindingLayout;
	std::unique_ptr<RayTracingPipeline> m_pipeline;
};
