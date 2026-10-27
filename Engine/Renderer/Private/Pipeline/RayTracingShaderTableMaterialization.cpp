#include "PCH.h"

#include "Pipeline/RayTracingPipelineRuntime.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "Scene/RayTracing/RayTracingShaderTablePlan.h"

std::vector<const RayTracingHitGroupComposition*> RayTracingPipelineRuntime::CollectRecordGroups(const RayTracingPipelineComposition& composition)
{
	std::vector<const RayTracingHitGroupComposition*> recordGroups;
	recordGroups.reserve(composition.GetHitGroups().size());
	for (const RayTracingHitGroupComposition& group : composition.GetHitGroups())
	{
		recordGroups.push_back(&group);
	}
	return recordGroups;
}

std::vector<const RayTracingHitGroupComposition*> RayTracingPipelineRuntime::ResolveSceneRecordGroups(const RayTracingPipelineComposition& composition, const RayTracingShaderTablePlan& plan)
{
	const RayTracingHitGroupComposition* opaqueGroup = nullptr;
	const RayTracingHitGroupComposition* alphaTestedGroup = nullptr;
	for (const RayTracingHitGroupComposition& group : composition.GetHitGroups())
	{
		if (group.AnyHit == 0u)
		{
			if (opaqueGroup != nullptr)
			{
				throw Diagnostics::Error("Scene shader-table composition contains multiple opaque hit groups.");
			}
			opaqueGroup = &group;
		}
		else
		{
			if (alphaTestedGroup != nullptr)
			{
				throw Diagnostics::Error("Scene shader-table composition contains multiple alpha-tested hit groups.");
			}
			alphaTestedGroup = &group;
		}
	}
	if (opaqueGroup == nullptr || alphaTestedGroup == nullptr)
	{
		throw Diagnostics::Error("Scene shader-table materialization requires one opaque and one alpha-tested hit group.");
	}

	std::vector<const RayTracingHitGroupComposition*> recordGroups;
	recordGroups.reserve(plan.GetRecords().size());
	for (const RayTracingShaderTableRecordPlan& record : plan.GetRecords())
	{
		recordGroups.push_back(record.HitGroup == RayTracingShaderTableHitGroup::AlphaTested ? alphaTestedGroup : opaqueGroup);
	}
	return recordGroups;
}

std::vector<RhiRayTracingShaderRecord> RayTracingPipelineRuntime::BuildShaderRecords(std::span<const ShaderTypeId> shaderTypes)
{
	std::vector<RhiRayTracingShaderRecord> records;
	records.reserve(shaderTypes.size());
	for (ShaderTypeId shaderType : shaderTypes)
	{
		records.push_back(RhiRayTracingShaderRecord{.ExportName = GetRegistration(shaderType).EntryPoint});
	}
	return records;
}

std::vector<RhiRayTracingShaderRecord> RayTracingPipelineRuntime::BuildHitGroupRecords(std::span<const RayTracingHitGroupComposition* const> recordGroups)
{
	std::vector<RhiRayTracingShaderRecord> hitGroupRecords;
	hitGroupRecords.reserve(recordGroups.size());
	for (const RayTracingHitGroupComposition* group : recordGroups)
	{
		if (group == nullptr)
		{
			throw Diagnostics::Error("Ray-tracing shader-table record has no hit-group composition.");
		}

		hitGroupRecords.push_back(
		    RhiRayTracingShaderRecord{.ExportName = group->ExportName, .LocalData = group->LocalData, .LocalRecordSignature = GetRegistration(group->ClosestHit).RayTracing.LocalRecordSignature});
	}
	return hitGroupRecords;
}

std::unique_ptr<RayTracingShaderTable> RayTracingPipelineRuntime::CreateShaderTable(
    RenderHardwareInterface& renderHardwareInterface,
    const RayTracingPipelineComposition& composition,
    std::span<const RayTracingHitGroupComposition* const> recordGroups) const
{
	if (m_pipeline == nullptr || composition.GetRayGeneration() == 0u)
	{
		throw Diagnostics::Error("Ray-tracing shader-table materialization received an invalid pipeline composition.");
	}
	const ShaderRegistrationDesc& rayGenerationRegistration = GetRegistration(composition.GetRayGeneration());
	std::vector<RhiRayTracingShaderRecord> rayGenerationRecords{RhiRayTracingShaderRecord{.ExportName = rayGenerationRegistration.EntryPoint}};
	const std::vector<RhiRayTracingShaderRecord> missRecords = BuildShaderRecords(composition.GetMissShaders());
	const std::vector<RhiRayTracingShaderRecord> hitGroupRecords = BuildHitGroupRecords(recordGroups);
	const std::vector<RhiRayTracingShaderRecord> callableRecords = BuildShaderRecords(composition.GetCallableShaders());
	std::wstring debugName = Strings::ToWide(rayGenerationRegistration.ShaderName);

	RayTracingShaderTableDesc tableDesc{
	    .Pipeline = m_pipeline.get(),
	    .RayGenerationRecords = rayGenerationRecords,
	    .MissRecords = missRecords,
	    .HitGroupRecords = hitGroupRecords,
	    .CallableRecords = callableRecords,
	    .Generation = m_generation,
	    .DebugName = debugName.c_str()};

	std::unique_ptr<RayTracingShaderTable> shaderTable = renderHardwareInterface.GetRayTracingService().CreateRayTracingShaderTable(tableDesc);
	if (shaderTable == nullptr)
	{
		throw Diagnostics::Error("Ray-tracing shader-table materialization failed.");
	}
	return shaderTable;
}
