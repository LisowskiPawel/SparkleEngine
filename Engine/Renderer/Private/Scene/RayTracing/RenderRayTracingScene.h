#pragma once

#include "RayTracing/RayTracingExecutionFrontend.h"
#include "Scene/RayTracing/RenderRayTracingFrameBindings.h"
#include "Scene/RayTracing/RayTracingShaderTablePlan.h"

#include <cstdint>
#include <memory>
#include <span>

class RenderCommandContext;
class RenderHardwareInterface;
class GpuMeshCache;
class PassExecutionDiagnostics;
class RayTracingBlasCache;
class RayTracingAccelerationStrategy;
struct PreparedRenderScene;
struct RenderMaterialTable;
struct RenderPrimitive;
struct RayTracingPtlasPartitionPlan;
struct RayTracingCapabilityReport;

class RenderRayTracingScene final
{
public:
	RenderRayTracingScene(RenderHardwareInterface& renderHardwareInterface, const GpuMeshCache& meshes, const RayTracingCapabilityReport& capabilityReport) noexcept;
	~RenderRayTracingScene() noexcept;

	RenderRayTracingScene(const RenderRayTracingScene&) = delete;
	RenderRayTracingScene& operator=(const RenderRayTracingScene&) = delete;
	RenderRayTracingScene(RenderRayTracingScene&&) = delete;
	RenderRayTracingScene& operator=(RenderRayTracingScene&&) = delete;

	RenderRayTracingFrameBindings Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept;
	void Build(RenderCommandContext& commandContext, const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan, PassExecutionDiagnostics* diagnostics = nullptr) noexcept;
	void Clear() noexcept;
	void SynchronizeShaderTablePlan(std::span<const RenderPrimitive> primitives, const RenderMaterialTable& materials) noexcept;

	RayTracingExecutionFrontend GetExecutionFrontend() const noexcept { return m_executionFrontend; }

	std::uint64_t GetGraphGeneration() const noexcept;

	RayTracingShaderTablePlan& GetShaderTablePlan() noexcept { return m_shaderTablePlan; }

private:
	RayTracingExecutionFrontend m_executionFrontend = RayTracingExecutionFrontend::None;
	RayTracingShaderTablePlan m_shaderTablePlan;
	std::unique_ptr<RayTracingBlasCache> m_blasCache;
	std::unique_ptr<RayTracingAccelerationStrategy> m_accelerationStrategy;
};
