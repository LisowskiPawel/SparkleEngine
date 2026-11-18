#pragma once

#include <cstdint>
#include <span>
#include <vector>

struct RenderMaterialTable;
struct RenderPrimitive;

enum class RayTracingSceneRayType : std::uint32_t
{
	Surface = 0u,
	Count = 1u,
};

enum class RayTracingShaderTableHitGroup : std::uint8_t
{
	Opaque,
	AlphaTested,
};

struct RayTracingShaderTableInstancePlan final
{
	std::uint32_t GpuSceneSlot = 0u;
	std::uint32_t InstanceContribution = 0u;
	std::uint32_t GeometryCount = 0u;
	std::uint64_t GeometryIdentity = 0u;
	RayTracingShaderTableHitGroup HitGroup = RayTracingShaderTableHitGroup::Opaque;

	bool operator==(const RayTracingShaderTableInstancePlan&) const noexcept = default;
};

class RayTracingShaderTablePlan final
{
public:
	void Synchronize(std::span<const RenderPrimitive> primitives, const RenderMaterialTable& materials) noexcept;
	void Clear() noexcept;

	bool ResolveInstanceContribution(std::uint32_t gpuSceneSlot, std::uint32_t& contribution) const noexcept;

	std::span<const RayTracingShaderTableHitGroup> GetHitGroups() const noexcept { return m_hitGroups; }

	std::uint64_t GetGeneration() const noexcept { return m_generation; }

private:
	const RayTracingShaderTableInstancePlan* FindInstance(std::uint32_t gpuSceneSlot) const noexcept;

	std::vector<RayTracingShaderTableInstancePlan> m_instances;
	std::vector<RayTracingShaderTableHitGroup> m_hitGroups;
	std::uint64_t m_generation = 0u;
};
