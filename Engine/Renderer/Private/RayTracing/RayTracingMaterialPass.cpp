#include "PCH.h"
#include "RayTracing/RayTracingMaterialPass.h"

std::vector<const RayTracingHitGroupComposition*> ResolveRayTracingMaterialRecordGroups(const RayTracingPipelineComposition& composition, const RayTracingShaderTablePlan& plan)
{
	if (!plan.Validate())
	{
		throw Diagnostics::Error("Ray-tracing shader-table materialization received an invalid scene plan.");
	}

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
