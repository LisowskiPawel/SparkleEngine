#include "PCH.h"

#include "Pipeline/RayTracingPipelineRuntime.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "PipelineRuntime/PipelineRuntimeLibrary.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/Shaders/ShaderParameterLayoutBuilder.h"

#include <string_view>
#include <unordered_set>

static std::vector<ShaderTypeId> CollectRayTracingShaderTypes(const RayTracingPipelineComposition& composition)
{
	std::vector<ShaderTypeId> selections;
	std::unordered_set<ShaderTypeId> selectedTypes;

	auto select = [&selections, &selectedTypes](ShaderTypeId shaderType)
	{
		if (selectedTypes.insert(shaderType).second)
		{
			selections.push_back(shaderType);
		}
	};

	select(composition.GetRayGeneration());
	for (ShaderTypeId miss : composition.GetMissShaders())
	{
		select(miss);
	}
	for (const RayTracingHitGroupComposition& group : composition.GetHitGroups())
	{
		select(group.ClosestHit);
		if (group.AnyHit != 0)
		{
			select(group.AnyHit);
		}
		if (group.Intersection != 0)
		{
			select(group.Intersection);
		}
	}
	for (ShaderTypeId callable : composition.GetCallableShaders())
	{
		select(callable);
	}
	return selections;
}

const ShaderRegistrationDesc& RayTracingPipelineRuntime::GetRegistration(ShaderTypeId shaderType)
{
	const ShaderRegistrationDesc* registration = GlobalShaderRegistry::FindById(shaderType);
	if (registration == nullptr)
	{
		throw Diagnostics::Error("Ray-tracing composition references an unregistered shader type.");
	}
	return *registration;
}

std::vector<RhiRayTracingShaderExportDesc> RayTracingPipelineRuntime::ResolveShaders(
    RenderHardwareInterface& renderHardwareInterface,
    const GlobalShaderMap& map,
    const CookedShaderLibrary& library,
    ShaderTarget target,
    const RayTracingPipelineComposition& composition)
{
	const std::vector<ShaderTypeId> selections = CollectRayTracingShaderTypes(composition);

	// Export descriptors borrow these shader addresses through native pipeline creation.
	m_shaders.reserve(selections.size());
	std::vector<RhiRayTracingShaderExportDesc> exports;
	exports.reserve(selections.size());
	for (ShaderTypeId selection : selections)
	{
		const ShaderRegistrationDesc& registration = GetRegistration(selection);
		const GlobalShaderMapEntry* entry = map.Find(selection, target);
		const CookedShaderCodeRecord* code = entry != nullptr ? library.Find(entry->CodeHash) : nullptr;
		m_shaders.push_back(ResolvedShader{.Map = &map, .Library = &library, .Entry = entry, .Code = code});
		exports.push_back(RhiRayTracingShaderExportDesc{.Shader = &m_shaders.back(), .ExportName = registration.EntryPoint});
		PipelineRuntimeLibrary::ValidateShaderCapabilities(renderHardwareInterface, registration.ShaderName, m_shaders.back());
	}

	return exports;
}

std::vector<RhiRayTracingHitGroupDesc> RayTracingPipelineRuntime::BuildHitGroups(const RayTracingPipelineComposition& composition)
{
	std::vector<RhiRayTracingHitGroupDesc> hitGroups;
	hitGroups.reserve(composition.GetHitGroups().size());
	for (const RayTracingHitGroupComposition& group : composition.GetHitGroups())
	{
		const ShaderRegistrationDesc& closestHit = GetRegistration(group.ClosestHit);
		const ShaderRegistrationDesc* anyHit = group.AnyHit != 0 ? &GetRegistration(group.AnyHit) : nullptr;
		const ShaderRegistrationDesc* intersection = group.Intersection != 0 ? &GetRegistration(group.Intersection) : nullptr;

		hitGroups.push_back(
		    RhiRayTracingHitGroupDesc{
		        .ExportName = group.ExportName,
		        .Kind = group.Kind,
		        .ClosestHitExport = closestHit.EntryPoint,
		        .AnyHitExport = anyHit != nullptr ? anyHit->EntryPoint : std::string_view{},
		        .IntersectionExport = intersection != nullptr ? intersection->EntryPoint : std::string_view{}});
	}
	return hitGroups;
}

void RayTracingPipelineRuntime::MaterializePipeline(
    RenderHardwareInterface& renderHardwareInterface,
    const RayTracingPipelineComposition& composition,
    std::span<const RhiRayTracingShaderExportDesc> exports)
{
	const ShaderRegistrationDesc& rayGenerationRegistration = GetRegistration(composition.GetRayGeneration());
	if (rayGenerationRegistration.BuildParameterStructDescriptor == nullptr)
	{
		throw Diagnostics::Error("Ray-generation shader has no registered global parameter contract.");
	}
	m_parameterLayout = BuildShaderParameterLayout(rayGenerationRegistration);
	std::wstring debugName = Strings::ToWide(rayGenerationRegistration.ShaderName);
	m_bindingLayout = PipelineRuntimeLibrary::CreateBindingLayout(renderHardwareInterface, m_parameterLayout, m_shaders, false, debugName.c_str());

	const std::vector<RhiRayTracingHitGroupDesc> hitGroups = BuildHitGroups(composition);
	const RayTracingShaderMetadata& rayGenerationMetadata = rayGenerationRegistration.RayTracing;

	RayTracingPipelineDesc pipelineDesc{
	    .GlobalBindingLayout = m_bindingLayout.get(),
	    .ShaderExports = exports,
	    .HitGroups = hitGroups,
	    .MaxPayloadSizeInBytes = rayGenerationMetadata.PayloadSizeInBytes,
	    .MaxAttributeSizeInBytes = rayGenerationMetadata.AttributeSizeInBytes,
	    .MaxRecursionDepth = rayGenerationMetadata.MinimumRecursionDepth,
	    .Generation = m_generation,
	    .DebugName = debugName.c_str()};

	m_pipeline = renderHardwareInterface.GetPipelineService().CreateRayTracingPipeline(pipelineDesc);
	if (m_pipeline == nullptr)
	{
		throw Diagnostics::Error("Ray-tracing native pipeline materialization failed.");
	}
}
