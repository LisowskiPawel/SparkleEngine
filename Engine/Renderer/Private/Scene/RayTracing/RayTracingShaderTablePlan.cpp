#include "PCH.h"

#include "Scene/RayTracing/RayTracingShaderTablePlan.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Rendering/RenderSceneDelta.h"
#include "RHI/Public/RayTracing/RhiAccelerationStructureDesc.h"
#include "Scene/RenderPrimitive.h"

#include <algorithm>
#include <limits>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rayTracingShaderTablePlanLogger, "Renderer.RayTracing.ShaderTablePlan");

static bool IsShaderTablePrimitiveAdmitted(const RenderPrimitive& primitive, const RenderMaterialTable& materials) noexcept
{
	if (!primitive.bGpuMeshResident || !primitive.GpuMesh)
	{
		return false;
	}
	if (!primitive.Static.Material.IsValid() || primitive.Static.Material.GetGeneration() != materials.Generation)
	{
		return false;
	}
	return primitive.Static.Material.GetIndex() < materials.Values.size();
}

static std::vector<RayTracingShaderTableInstancePlan> CollectShaderTableInstances(std::span<const RenderPrimitive> primitives, const RenderMaterialTable& materials)
{
	std::vector<RayTracingShaderTableInstancePlan> instances;
	instances.reserve(primitives.size());
	for (const RenderPrimitive& primitive : primitives)
	{
		if (!IsShaderTablePrimitiveAdmitted(primitive, materials))
		{
			continue;
		}

		const std::uint32_t materialIndex = primitive.Static.Material.GetIndex();

		const RayTracingShaderTableHitGroup hitGroup = materials.Values[materialIndex].alphaMode == AlphaMode::Mask ? RayTracingShaderTableHitGroup::AlphaTested
		                                                                                                            : RayTracingShaderTableHitGroup::Opaque;

		instances.push_back(RayTracingShaderTableInstancePlan{.GpuSceneSlot = primitive.GpuSceneSlot, .GeometryCount = 1u, .GeometryIdentity = primitive.GpuMesh.Value, .HitGroup = hitGroup});
	}
	std::ranges::sort(instances, {}, &RayTracingShaderTableInstancePlan::GpuSceneSlot);

	return instances;
}

static std::uint32_t AssignInstanceContributions(std::span<RayTracingShaderTableInstancePlan> instances) noexcept
{
	const std::uint32_t geometryMultiplier = static_cast<std::uint32_t>(RayTracingSceneRayType::Count);
	std::uint64_t nextInstanceContribution = 0u;
	for (RayTracingShaderTableInstancePlan& instance : instances)
	{
		if (nextInstanceContribution > kRhiRayTracingMaxInstanceContributionToHitGroupIndex)
		{
			Diagnostics::Fatal(g_rayTracingShaderTablePlanLogger, __FILE__, __LINE__, "Ray-tracing shader-table instance contribution exceeds the RHI descriptor contract.");
		}
		instance.InstanceContribution = static_cast<std::uint32_t>(nextInstanceContribution);
		nextInstanceContribution += static_cast<std::uint64_t>(geometryMultiplier) * instance.GeometryCount;
	}
	if (nextInstanceContribution > std::numeric_limits<std::uint32_t>::max())
	{
		Diagnostics::Fatal(g_rayTracingShaderTablePlanLogger, __FILE__, __LINE__, "Ray-tracing shader-table record count exceeds the Renderer/RHI index width.");
	}

	return static_cast<std::uint32_t>(nextInstanceContribution);
}

static std::vector<RayTracingShaderTableHitGroup> BuildShaderTableHitGroups(std::span<const RayTracingShaderTableInstancePlan> instances, std::uint32_t recordCount)
{
	const std::uint32_t geometryMultiplier = static_cast<std::uint32_t>(RayTracingSceneRayType::Count);
	std::vector<RayTracingShaderTableHitGroup> hitGroups;
	hitGroups.reserve(recordCount);
	for (const RayTracingShaderTableInstancePlan& instance : instances)
	{
		hitGroups.insert(hitGroups.end(), static_cast<std::size_t>(geometryMultiplier) * instance.GeometryCount, instance.HitGroup);
	}
	return hitGroups;
}

void RayTracingShaderTablePlan::Synchronize(std::span<const RenderPrimitive> primitives, const RenderMaterialTable& materials) noexcept
{
	auto instances = CollectShaderTableInstances(primitives, materials);
	const std::uint32_t recordCount = AssignInstanceContributions(instances);
	if (std::ranges::equal(instances, m_instances))
	{
		return;
	}

	m_hitGroups = BuildShaderTableHitGroups(instances, recordCount);
	m_instances = std::move(instances);
	++m_generation;
}

void RayTracingShaderTablePlan::Clear() noexcept
{
	m_instances.clear();
	m_hitGroups.clear();
	++m_generation;
}

bool RayTracingShaderTablePlan::ResolveInstanceContribution(std::uint32_t gpuSceneSlot, std::uint32_t& contribution) const noexcept
{
	const RayTracingShaderTableInstancePlan* instance = FindInstance(gpuSceneSlot);
	if (instance == nullptr)
	{
		return false;
	}
	contribution = instance->InstanceContribution;
	return true;
}

const RayTracingShaderTableInstancePlan* RayTracingShaderTablePlan::FindInstance(std::uint32_t gpuSceneSlot) const noexcept
{
	const auto instance = std::ranges::lower_bound(m_instances, gpuSceneSlot, {}, &RayTracingShaderTableInstancePlan::GpuSceneSlot);
	return instance != m_instances.end() && instance->GpuSceneSlot == gpuSceneSlot ? &*instance : nullptr;
}
