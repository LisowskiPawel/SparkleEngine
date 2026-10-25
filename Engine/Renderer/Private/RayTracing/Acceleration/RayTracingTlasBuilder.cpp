#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingTlasBuilder.h"
#include "RayTracing/Acceleration/RayTracingBlasCache.h"

#include "Commands/RenderCommandContext.h"
#include "Core/Public/Math/MathUtils.h"
#include "Meshes/GpuMesh.h"
#include "Debug/RendererCVars.h"
#include "RayTracing/Diagnostics/RayTracingPerformanceDiagnostics.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/RayTracing/RhiRayTracingTransformPacking.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "Scene/RayTracing/RayTracingShaderTablePlan.h"

#include <algorithm>
#include <unordered_set>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rayTracingTlasBuilderLogger, "Renderer.RayTracing.TlasBuilder");

struct RayTracingTlasBuilder::BuildState final
{
	std::vector<RhiRayTracingInstanceDesc> Instances;
	std::unordered_set<void*> BuiltBlasResources;
	RhiRayTracingAccelerationStructurePrebuildInfo PrebuildInfo = {};
	ERhiTlasBuildFlags RequestedFlags = ERhiTlasBuildFlags::None;
	ERhiTlasBuildMode Mode = ERhiTlasBuildMode::Build;
	const char* EventName = "TLAS Build";
};

std::uint64_t RayTracingTlasBuilder::AlignRayTracingBufferSize(std::uint64_t sizeInBytes, std::uint64_t alignment) noexcept
{
	return alignment > 0 ? MathUtils::AlignUp(sizeInBytes, alignment) : sizeInBytes;
}

bool RayTracingTlasBuilder::SupportsTlasRefit(RenderHardwareInterface& renderHardwareInterface) noexcept
{
	return renderHardwareInterface.GetCapabilities().RayTracing.Groups.Tlas.SupportsTlasUpdate;
}

ERhiTlasBuildFlags RayTracingTlasBuilder::ResolveTlasBuildFlags(RenderHardwareInterface& renderHardwareInterface) noexcept
{
	return CVarRayTracingTlasRefit.Get() && SupportsTlasRefit(renderHardwareInterface) ? ERhiTlasBuildFlags::AllowUpdate : ERhiTlasBuildFlags::None;
}

std::uint64_t RayTracingTlasBuilder::ResolveScratchSize(const RhiRayTracingAccelerationStructurePrebuildInfo& prebuildInfo, ERhiTlasBuildFlags buildFlags) noexcept
{
	if (!HasFlag(buildFlags, ERhiTlasBuildFlags::AllowUpdate))
	{
		return prebuildInfo.ScratchDataSizeInBytes;
	}
	return (std::max) (prebuildInfo.ScratchDataSizeInBytes, prebuildInfo.UpdateScratchDataSizeInBytes);
}

RhiRayTracingInstanceFlags RayTracingTlasBuilder::ResolveInstanceFlags(const PreparedRenderScene& preparedScene, const MeshDraw& draw) noexcept
{
	RhiRayTracingInstanceFlags flags = RhiRayTracingInstanceFlags::None;
	if (draw.MaterialSlot >= preparedScene.materials.size())
	{
		Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS input references a material outside the render scene.");
	}
	const MaterialData& material = preparedScene.materials[draw.MaterialSlot];
	if (material.doubleSided)
	{
		flags = flags | RhiRayTracingInstanceFlags::TriangleFacingCullDisable;
	}
	if (material.alphaMode == 1u)
	{
		flags = flags | RhiRayTracingInstanceFlags::ForceNonOpaque;
	}
	return flags;
}

RayTracingTlasBuilder::RayTracingTlasBuilder(RenderHardwareInterface& renderHardwareInterface) noexcept :
    m_renderHardwareInterface(renderHardwareInterface)
{
}

RayTracingTlasBuilder::~RayTracingTlasBuilder() noexcept
{
	Clear();
}

void RayTracingTlasBuilder::Prepare(std::uint32_t instanceCapacity) noexcept
{
	const ERhiTlasBuildFlags buildFlags = ResolveTlasBuildFlags(m_renderHardwareInterface);
	const RhiRayTracingAccelerationStructurePrebuildInfo prebuildInfo = m_renderHardwareInterface.GetRayTracingService().GetTlasPrebuildInfo(instanceCapacity, buildFlags);
	if (prebuildInfo.ResultDataMaxSizeInBytes == 0 || ResolveScratchSize(prebuildInfo, buildFlags) == 0)
	{
		Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS prebuild sizing produced an unusable resource layout.");
	}

	EnsureResources(prebuildInfo, buildFlags);

	if (m_renderHardwareInterface.GetResourceService().GetResourceGpuVirtualAddress(m_accelerationStructureBuffer) == 0)
	{
		Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS storage has no GPU address.");
	}
}

std::uint32_t RayTracingTlasBuilder::Build(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    RayTracingPerformanceDiagnostics* diagnostics) noexcept
{
	const auto& instances = preparedScene.rayTracingInstances;
	BuildState state;
	state.Instances.reserve(instances.size());
	CollectInstances(commandContext, preparedScene, blasCache, shaderTablePlan, diagnostics, state);
	const std::uint32_t instanceCount = static_cast<std::uint32_t>(state.Instances.size());
	PrepareBuild(state);

	RecordBuild(commandContext, state, diagnostics);

	m_builtInstanceCount = instanceCount;
	return instanceCount;
}

void RayTracingTlasBuilder::CollectInstances(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    RayTracingPerformanceDiagnostics* diagnostics,
    BuildState& state) noexcept
{
	const auto& instances = preparedScene.rayTracingInstances;
	for (const RenderRayTracingInstance& input : instances)
	{
		if (input.PrimitiveIndex >= preparedScene.primitives.size())
		{
			Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS work references a mesh instance outside the render scene.");
		}
		const MeshDraw& draw = preparedScene.primitives[input.PrimitiveIndex].Draw;
		if (!draw.Geometry.Mesh)
		{
			Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS work references a mesh instance with no GPU mesh handle.");
		}

		const RayTracingBlasCache::BlasHandle blas = blasCache.EnsureBlas(commandContext, preparedScene, draw, input.GpuSceneSlot, diagnostics);

		commandContext.TrackResource(blas.resource);
		if (blas.builtThisFrame)
		{
			state.BuiltBlasResources.insert(blas.resource.Value);
		}
		std::uint32_t instanceContribution = 0u;
		if (!shaderTablePlan.ResolveInstanceContribution(input.GpuSceneSlot, instanceContribution))
		{
			Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS instance has no authoritative scene shader-table contribution.");
		}

		state.Instances.push_back(
		    RhiRayTracingInstanceDesc{
		        .Transform = RhiRayTracingTransformPacking::PackCanonicalObjectToWorld(draw.Transform.WorldMatrix),
		        .InstanceID = input.GpuSceneSlot,
		        .InstanceMask = 0xFFu,
		        .InstanceContributionToHitGroupIndex = instanceContribution,
		        .Flags = ResolveInstanceFlags(preparedScene, draw),
		        .AccelerationStructure = blas.gpuAddress});
	}
}

void RayTracingTlasBuilder::PrepareBuild(BuildState& state) noexcept
{
	state.RequestedFlags = m_resourcesAllowUpdate ? ERhiTlasBuildFlags::AllowUpdate : ERhiTlasBuildFlags::None;
	state.PrebuildInfo = m_renderHardwareInterface.GetRayTracingService().GetTlasPrebuildInfo(static_cast<std::uint32_t>(state.Instances.size()), state.RequestedFlags);
	const std::uint64_t scratchSizeInBytes = ResolveScratchSize(state.PrebuildInfo, state.RequestedFlags);
	if (state.PrebuildInfo.ResultDataMaxSizeInBytes == 0 || scratchSizeInBytes == 0)
	{
		Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS build sizing produced an unusable resource layout.");
	}
	EnsureResources(state.PrebuildInfo, state.RequestedFlags);

	RhiResourceService& resourceService = m_renderHardwareInterface.GetResourceService();
	if (m_instanceBuffer)
	{
		resourceService.ReleaseOwnedResource(m_instanceBuffer);
		m_instanceBuffer = {};
	}

	m_instanceBuffer = m_renderHardwareInterface.GetRayTracingService().CreateRayTracingInstanceBuffer(
	    state.Instances.empty() ? nullptr : state.Instances.data(),
	    static_cast<std::uint32_t>(state.Instances.size()),
	    L"RayTracingTlasInstances");

	if (!m_instanceBuffer)
	{
		Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS instance-buffer allocation failed.");
	}

	const bool canRefit = !state.Instances.empty() && m_resourcesAllowUpdate && m_builtInstanceCount == state.Instances.size() && state.PrebuildInfo.UpdateScratchDataSizeInBytes > 0;

	state.Mode = canRefit ? ERhiTlasBuildMode::Update : (HasFlag(state.RequestedFlags, ERhiTlasBuildFlags::AllowUpdate) ? ERhiTlasBuildMode::BuildAllowUpdate : ERhiTlasBuildMode::Build);

	state.EventName = canRefit ? "TLAS Refit" : "TLAS Build";
}

void RayTracingTlasBuilder::RecordBuild(RenderCommandContext& commandContext, const BuildState& state, RayTracingPerformanceDiagnostics* diagnostics) const noexcept
{
	for (void* resourceValue : state.BuiltBlasResources)
	{
		commandContext.UnorderedAccessBarrier(RhiResourceHandle{resourceValue});
	}
	TrackBuildResources(commandContext);

	auto tlasGpuScope = diagnostics != nullptr ? diagnostics->BeginGpuScope(state.EventName) : ScopedGpuScope{};
	RhiResourceService& resourceService = m_renderHardwareInterface.GetResourceService();

	commandContext.BuildTlas(
	    resourceService.GetResourceGpuVirtualAddress(m_instanceBuffer),
	    static_cast<std::uint32_t>(state.Instances.size()),
	    resourceService.GetResourceGpuVirtualAddress(m_scratchBuffer),
	    resourceService.GetResourceGpuVirtualAddress(m_accelerationStructureBuffer),
	    state.Mode);
}

void RayTracingTlasBuilder::TrackBuildResources(RenderCommandContext& commandContext) const noexcept
{
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();

	commandContext.TrackResource(resources.GetResourceHandle(m_instanceBuffer));
	commandContext.TrackResource(resources.GetResourceHandle(m_scratchBuffer));
	commandContext.TrackResource(resources.GetResourceHandle(m_accelerationStructureBuffer));
}

void RayTracingTlasBuilder::Clear() noexcept
{
	ReleaseResources();
}

void RayTracingTlasBuilder::ReleaseResources() noexcept
{
	if (m_instanceBuffer)
	{
		m_renderHardwareInterface.GetResourceService().ReleaseOwnedResource(m_instanceBuffer);
	}
	if (m_scratchBuffer)
	{
		m_renderHardwareInterface.GetResourceService().ReleaseOwnedResource(m_scratchBuffer);
	}
	if (m_accelerationStructureBuffer)
	{
		m_renderHardwareInterface.GetResourceService().ReleaseOwnedResource(m_accelerationStructureBuffer);
	}

	m_instanceBuffer = {};
	m_scratchBuffer = {};
	m_accelerationStructureBuffer = {};
	m_scratchBufferSizeInBytes = 0;
	m_accelerationStructureSizeInBytes = 0;
	m_resourcesAllowUpdate = false;
	m_builtInstanceCount.reset();
}

void RayTracingTlasBuilder::EnsureResources(const RhiRayTracingAccelerationStructurePrebuildInfo& prebuildInfo, ERhiTlasBuildFlags buildFlags) noexcept
{
	const bool requestedAllowUpdate = HasFlag(buildFlags, ERhiTlasBuildFlags::AllowUpdate);
	const std::uint64_t scratchSizeInBytes = ResolveScratchSize(prebuildInfo, buildFlags);
	if (m_resourcesAllowUpdate != requestedAllowUpdate)
	{
		if (m_scratchBuffer)
		{
			m_renderHardwareInterface.GetResourceService().ReleaseOwnedResource(m_scratchBuffer);
			m_scratchBuffer = {};
			m_scratchBufferSizeInBytes = 0;
		}
		if (m_accelerationStructureBuffer)
		{
			m_renderHardwareInterface.GetResourceService().ReleaseOwnedResource(m_accelerationStructureBuffer);
			m_accelerationStructureBuffer = {};
			m_accelerationStructureSizeInBytes = 0;
		}
		m_builtInstanceCount.reset();
		m_resourcesAllowUpdate = requestedAllowUpdate;
	}

	if (m_scratchBuffer && m_scratchBufferSizeInBytes < scratchSizeInBytes)
	{
		m_renderHardwareInterface.GetResourceService().ReleaseOwnedResource(m_scratchBuffer);
		m_scratchBuffer = {};
		m_scratchBufferSizeInBytes = 0;
	}
	if (m_accelerationStructureBuffer && m_accelerationStructureSizeInBytes < prebuildInfo.ResultDataMaxSizeInBytes)
	{
		m_builtInstanceCount.reset();
		m_renderHardwareInterface.GetResourceService().ReleaseOwnedResource(m_accelerationStructureBuffer);
		m_accelerationStructureBuffer = {};
		m_accelerationStructureSizeInBytes = 0;
	}

	if (!m_scratchBuffer)
	{
		const std::uint64_t alignedScratchSize = AlignRayTracingBufferSize(scratchSizeInBytes, m_renderHardwareInterface.GetCapabilities().RayTracing.ScratchBufferByteAlignment);
		m_scratchBuffer = m_renderHardwareInterface.GetRayTracingService().CreateRayTracingScratchBuffer(alignedScratchSize, L"RayTracingTlasScratch");
		m_scratchBufferSizeInBytes = alignedScratchSize;
	}
	if (!m_accelerationStructureBuffer)
	{
		const std::uint64_t alignedAccelerationStructureSize = AlignRayTracingBufferSize(
		    prebuildInfo.ResultDataMaxSizeInBytes,
		    m_renderHardwareInterface.GetCapabilities().RayTracing.AccelerationStructureByteAlignment);

		m_accelerationStructureBuffer = m_renderHardwareInterface.GetRayTracingService().CreateRayTracingAccelerationStructureBuffer(
		    alignedAccelerationStructureSize,
		    ERhiRayTracingAccelerationStructureType::TopLevel,
		    L"RayTracingTlas");

		m_accelerationStructureSizeInBytes = alignedAccelerationStructureSize;
	}

	if (!m_scratchBuffer || !m_accelerationStructureBuffer)
	{
		Diagnostics::Fatal(g_rayTracingTlasBuilderLogger, __FILE__, __LINE__, "TLAS scratch or acceleration-structure allocation failed.");
	}
}
