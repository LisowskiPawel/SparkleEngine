#include "PCH.h"
#include "RayTracing/RayTracingMaterialPass.h"

std::vector<const RayTracingHitGroupComposition*> ResolveRayTracingMaterialRecordGroups(const RayTracingPipelineComposition& composition, const RayTracingShaderTablePlan& plan)
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
	recordGroups.reserve(plan.GetHitGroups().size());
	for (const RayTracingShaderTableHitGroup hitGroup : plan.GetHitGroups())
	{
		recordGroups.push_back(hitGroup == RayTracingShaderTableHitGroup::AlphaTested ? alphaTestedGroup : opaqueGroup);
	}
	return recordGroups;
}
