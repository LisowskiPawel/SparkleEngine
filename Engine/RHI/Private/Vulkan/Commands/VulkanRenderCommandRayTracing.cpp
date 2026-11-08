#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Commands/VulkanRenderCommandList.h"
#include "Vulkan/Pipeline/VulkanRayTracingPipeline.h"
#include "Vulkan/RayTracing/VulkanRayTracingShaderTable.h"

#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/RayTracing/VulkanAccelerationStructureCommands.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Validation/RhiContract.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanRenderCommandListLogger, "RHI.Vulkan.CommandList");

void VulkanRenderCommandList::SetRayTracingPipeline(const RayTracingPipeline& pipeline) noexcept
{
	const auto* nativePipeline = dynamic_cast<const VulkanRayTracingPipeline*>(&pipeline);
	if (m_commandBuffer == VK_NULL_HANDLE || nativePipeline == nullptr)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan ray-tracing pipeline binding received no command buffer or a foreign pipeline.");
	}
	m_rayTracingBindings.PipelineLayout = nativePipeline->GetPipelineLayout();
	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, nativePipeline->GetPipeline());
	m_boundRayTracingPipeline = &pipeline;
}

void VulkanRenderCommandList::BuildBottomLevelAccelerationStructure(const RhiRayTracingGeometryDesc& geometry, RhiGpuVirtualAddress scratchGpuAddress, RhiGpuVirtualAddress resultGpuAddress) noexcept
{
	const VkDeviceAddress vertexBufferAddress = ResolveRayTracingBufferAddress(geometry.VertexBuffer);
	const VkDeviceAddress indexBufferAddress = ResolveRayTracingBufferAddress(geometry.IndexBuffer);
	const bool bInvalidBuildInputsRhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bInvalidBuildInputsMemoryAllocator = bInvalidBuildInputsRhi || (m_memoryAllocator == nullptr);
	const bool bBuildInputsAccelerationStructures = bInvalidBuildInputsMemoryAllocator || (m_rhi->GetCmdBuildAccelerationStructures() == nullptr);
	const bool bInvalidBuildInputsRhiContract = bBuildInputsAccelerationStructures || (!RhiContract::IsRayTracingGeometryDescUsable(geometry));
	const bool bInvalidBuildInputsBufferAddress = bInvalidBuildInputsRhiContract || (vertexBufferAddress == 0);
	const bool bInvalidBufferAddresses = bInvalidBuildInputsBufferAddress || (indexBufferAddress == 0);
	const bool bInvalidBuildInputsGpuAddress = bInvalidBufferAddresses || (scratchGpuAddress == 0);
	const bool bInvalidBlasBuildInputs = bInvalidBuildInputsGpuAddress || (resultGpuAddress == 0);

	if (bInvalidBlasBuildInputs)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan BLAS build received incomplete geometry, device, command-buffer, or GPU-address inputs.");
	}
	EndDynamicRenderingIfNeeded();

	TrackResource(geometry.VertexBuffer.Resource);
	TrackResource(geometry.IndexBuffer.Resource);

	VulkanRecordingResource resultResource;
	const bool bResultResolved = ResolveAddress(resultGpuAddress, resultResource);
	const bool bStructureMatchesStructure = bResultResolved && resultResource.AccelerationStructure != VK_NULL_HANDLE;
	const bool bAccelerationStructureMatches = bStructureMatchesStructure && (resultResource.AccelerationStructureType == VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);

	if (!bAccelerationStructureMatches)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan BLAS destination does not resolve to a bottom-level acceleration structure.");
	}

	VulkanAccelerationStructureCommands::BuildBlas(
	    m_commandBuffer,
	    m_rhi->GetCmdBuildAccelerationStructures(),
	    geometry,
	    vertexBufferAddress,
	    indexBufferAddress,
	    scratchGpuAddress,
	    resultResource.AccelerationStructure);
}

void VulkanRenderCommandList::BuildTlas(
    RhiGpuVirtualAddress instanceDescsGpuAddress,
    std::uint32_t instanceCount,
    RhiGpuVirtualAddress scratchGpuAddress,
    RhiGpuVirtualAddress resultGpuAddress,
    ERhiTlasBuildMode buildMode) noexcept
{
	const bool bInvalidBuildInputsRhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bInvalidBuildInputsMemoryAllocator = bInvalidBuildInputsRhi || (m_memoryAllocator == nullptr);
	const bool bBuildInputsAccelerationStructures = bInvalidBuildInputsMemoryAllocator || (m_rhi->GetCmdBuildAccelerationStructures() == nullptr);
	const bool bInvalidBuildInputsGpuAddress = bBuildInputsAccelerationStructures || (instanceDescsGpuAddress == 0);
	const bool bInvalidScratchAddress = bInvalidBuildInputsGpuAddress || (scratchGpuAddress == 0);
	const bool bInvalidTlasBuildInputs = bInvalidScratchAddress || (resultGpuAddress == 0);

	if (bInvalidTlasBuildInputs)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TLAS build received no device, command buffer, build entry point, or GPU address.");
	}
	EndDynamicRenderingIfNeeded();

	VulkanRecordingResource resultResource;
	const bool bResultResolved = ResolveAddress(resultGpuAddress, resultResource);
	const bool bStructureMatchesStructure = bResultResolved && resultResource.AccelerationStructure != VK_NULL_HANDLE;
	const bool bAccelerationStructureMatches = bStructureMatchesStructure && (resultResource.AccelerationStructureType == VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR);

	if (!bAccelerationStructureMatches)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TLAS destination does not resolve to a TLAS.");
	}

	VulkanAccelerationStructureCommands::BuildTlas(
	    m_commandBuffer,
	    m_rhi->GetCmdBuildAccelerationStructures(),
	    instanceDescsGpuAddress,
	    instanceCount,
	    scratchGpuAddress,
	    resultResource.AccelerationStructure,
	    buildMode);
}

void VulkanRenderCommandList::BuildPtlas(const RhiPtlasBuildCommandDesc& desc) noexcept
{
	const bool bInvalidBuildInputsRhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bInvalidBuildInputsBuildPtlas = bInvalidBuildInputsRhi || (m_rhi->GetCmdBuildPtlas() == nullptr);
	const bool bBuildInputsDestinationResource = bInvalidBuildInputsBuildPtlas || (!desc.DestinationResource);
	const bool bBuildInputsAccelerationStructure = bBuildInputsDestinationResource || (desc.DestinationAccelerationStructure == 0);
	const bool bInvalidBuildInputsScratch = bBuildInputsAccelerationStructure || (desc.Scratch == 0);
	const bool bBuildInputsOperationHeaders = bInvalidBuildInputsScratch || (desc.OperationHeaders == 0);
	const bool bInvalidBuildInputsOperationCount = bBuildInputsOperationHeaders || (desc.OperationCount == 0);
	const bool bInvalidBuildInputsLayout = bInvalidBuildInputsOperationCount || (desc.Layout.InstanceCapacity == 0);
	const bool bInvalidPtlasBuildInputs = bInvalidBuildInputsLayout || (desc.Layout.PartitionCount == 0);

	if (bInvalidPtlasBuildInputs)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan PTLAS build received incomplete device, command-buffer, layout, or GPU-address inputs.");
	}
	EndDynamicRenderingIfNeeded();

	VulkanAccelerationStructureCommands::BuildPtlas(m_commandBuffer, m_rhi->GetCmdBuildPtlas(), desc);
}

void VulkanRenderCommandList::TraceRays(const TraceRaysDesc& desc) noexcept
{
	try
	{
		RhiContract::ValidateTraceRaysDesc(desc, m_queueType);
	}
	catch (const std::exception& error)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, error.what());
	}
	const auto* pipeline = dynamic_cast<const VulkanRayTracingPipeline*>(desc.Pipeline);
	const auto* table = dynamic_cast<const VulkanRayTracingShaderTable*>(desc.ShaderTable);
	const bool bRayDispatchUnavailableRhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bRayDispatchUnavailableTraceRays = bRayDispatchUnavailableRhi || (m_rhi->GetCmdTraceRays() == nullptr);
	const bool bRayDispatchUnavailablePipeline = bRayDispatchUnavailableTraceRays || (pipeline == nullptr);
	const bool bRayDispatchUnavailable = bRayDispatchUnavailablePipeline || (table == nullptr);

	if (bRayDispatchUnavailable)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TraceRays requires a command buffer, loaded function, and matching native objects.");
	}
	if (m_boundRayTracingPipeline != desc.Pipeline)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TraceRays requires its exact pipeline to be bound first.");
	}
	TrackResource(table->GetResource());
	const VkDeviceAddress baseAddress = table->GetDeviceAddress();

	const auto nativeRegion = [baseAddress](const RhiRayTracingShaderTableRegion& region)
	{
		return VkStridedDeviceAddressRegionKHR{.deviceAddress = region.SizeInBytes != 0 ? baseAddress + region.OffsetInBytes : 0, .stride = region.StrideInBytes, .size = region.SizeInBytes};
	};

	const VkStridedDeviceAddressRegionKHR rayGeneration = nativeRegion(desc.RayGeneration);
	const VkStridedDeviceAddressRegionKHR miss = nativeRegion(desc.Miss);
	const VkStridedDeviceAddressRegionKHR hitGroup = nativeRegion(desc.HitGroup);
	const VkStridedDeviceAddressRegionKHR callable = nativeRegion(desc.Callable);
	FlushRayTracingDescriptorSets();
	m_rhi->GetCmdTraceRays()(m_commandBuffer, &rayGeneration, &miss, &hitGroup, &callable, desc.Width, desc.Height, desc.Depth);
}

VkDeviceAddress VulkanRenderCommandList::ResolveRayTracingBufferAddress(const RhiRayTracingBufferBinding& binding) const noexcept
{
	VulkanRecordingResource resource;
	if (!ResolveResource(binding.Resource, resource) || resource.Buffer == VK_NULL_HANDLE || resource.BufferDeviceAddress == 0)
	{
		return 0;
	}

	return resource.BufferDeviceAddress + binding.OffsetInBytes;
}
