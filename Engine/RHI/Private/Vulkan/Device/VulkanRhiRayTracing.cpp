#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanRhi.h"

#include "CVars/RHICVars.h"

#include <limits>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanRhiLogger, "RHI.Vulkan");

RhiRayTracingCapabilities VulkanRhi::GetRayTracingCapabilities() const noexcept
{
	return m_rayTracingCapabilities;
}

PFN_vkGetBufferDeviceAddress VulkanRhi::GetGetBufferDeviceAddress() const noexcept
{
	return m_getBufferDeviceAddress;
}

PFN_vkCreateAccelerationStructureKHR VulkanRhi::GetCreateAccelerationStructure() const noexcept
{
	return m_createAccelerationStructure;
}

PFN_vkDestroyAccelerationStructureKHR VulkanRhi::GetDestroyAccelerationStructure() const noexcept
{
	return m_destroyAccelerationStructure;
}

PFN_vkGetAccelerationStructureBuildSizesKHR VulkanRhi::GetAccelerationStructureBuildSizes() const noexcept
{
	return m_getAccelerationStructureBuildSizes;
}

PFN_vkCmdBuildAccelerationStructuresKHR VulkanRhi::GetCmdBuildAccelerationStructures() const noexcept
{
	return m_cmdBuildAccelerationStructures;
}

PFN_vkGetAccelerationStructureDeviceAddressKHR VulkanRhi::GetAccelerationStructureDeviceAddress() const noexcept
{
	return m_getAccelerationStructureDeviceAddress;
}

PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV VulkanRhi::GetPtlasBuildSizes() const noexcept
{
	return m_getPtlasBuildSizes;
}

PFN_vkCmdBuildPartitionedAccelerationStructuresNV VulkanRhi::GetCmdBuildPtlas() const noexcept
{
	return m_cmdBuildPtlas;
}

PFN_vkCreateRayTracingPipelinesKHR VulkanRhi::GetCreateRayTracingPipelines() const noexcept
{
	return m_createRayTracingPipelines;
}

PFN_vkGetRayTracingShaderGroupHandlesKHR VulkanRhi::GetRayTracingShaderGroupHandles() const noexcept
{
	return m_getRayTracingShaderGroupHandles;
}

PFN_vkCmdTraceRaysKHR VulkanRhi::GetCmdTraceRays() const noexcept
{
	return m_cmdTraceRays;
}

void VulkanRhi::LoadRayTracingFunctions() noexcept
{
	if (m_device == VK_NULL_HANDLE || !m_featureStatus.RayTracing.bEnabledAccelerationStructure)
	{
		return;
	}

	m_getBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(vkGetDeviceProcAddr(m_device, "vkGetBufferDeviceAddress"));
	if (m_getBufferDeviceAddress == nullptr)
	{
		m_getBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(vkGetDeviceProcAddr(m_device, "vkGetBufferDeviceAddressKHR"));
	}
	m_createAccelerationStructure = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(m_device, "vkCreateAccelerationStructureKHR"));
	m_destroyAccelerationStructure = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(m_device, "vkDestroyAccelerationStructureKHR"));
	m_getAccelerationStructureBuildSizes = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(m_device, "vkGetAccelerationStructureBuildSizesKHR"));
	m_cmdBuildAccelerationStructures = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(m_device, "vkCmdBuildAccelerationStructuresKHR"));
	m_getAccelerationStructureDeviceAddress = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(m_device, "vkGetAccelerationStructureDeviceAddressKHR"));
	if (m_featureStatus.RayTracing.bEnabledRayTracingPipeline)
	{
		m_createRayTracingPipelines = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(m_device, "vkCreateRayTracingPipelinesKHR"));
		m_getRayTracingShaderGroupHandles = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(m_device, "vkGetRayTracingShaderGroupHandlesKHR"));
		m_cmdTraceRays = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(m_device, "vkCmdTraceRaysKHR"));
	}
	if (m_featureStatus.RayTracing.bEnabledPtlas)
	{
		m_getPtlasBuildSizes = reinterpret_cast<PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV>(vkGetDeviceProcAddr(m_device, "vkGetPartitionedAccelerationStructuresBuildSizesNV"));

		m_cmdBuildPtlas = reinterpret_cast<PFN_vkCmdBuildPartitionedAccelerationStructuresNV>(vkGetDeviceProcAddr(m_device, "vkCmdBuildPartitionedAccelerationStructuresNV"));
	}

	const bool bLoadedAccelerationStructure = m_getBufferDeviceAddress != nullptr && m_createAccelerationStructure != nullptr;
	const bool bLoadedAccelerationStructure2 = bLoadedAccelerationStructure && (m_destroyAccelerationStructure != nullptr);
	const bool bLoadedBuildSizes = bLoadedAccelerationStructure2 && (m_getAccelerationStructureBuildSizes != nullptr);
	const bool bLoadedAccelerationStructures = bLoadedBuildSizes && (m_cmdBuildAccelerationStructures != nullptr);
	const bool bLoaded = bLoadedAccelerationStructures && (m_getAccelerationStructureDeviceAddress != nullptr);

	if (!bLoaded)
	{
		m_featureStatus.RayTracing.bEnabledAccelerationStructure = false;
		m_featureStatus.RayTracing.bEnabledInlineRayQuery = false;
		m_featureStatus.RayTracing.bEnabledRayTracingPipeline = false;
		m_featureStatus.RayTracing.bEnabledPtlas = false;
	}
	if (m_createRayTracingPipelines == nullptr || m_getRayTracingShaderGroupHandles == nullptr || m_cmdTraceRays == nullptr)
	{
		m_featureStatus.RayTracing.bEnabledRayTracingPipeline = false;
	}
	if (m_featureStatus.RayTracing.bEnabledPtlas && (m_getPtlasBuildSizes == nullptr || m_cmdBuildPtlas == nullptr))
	{
		m_featureStatus.RayTracing.bEnabledPtlas = false;
	}
}

void VulkanRhi::BuildRayTracingCapabilities() noexcept
{
	m_rayTracingCapabilities = {};

	m_rayTracingCapabilities.Groups.Ptlas = RhiPtlasCapabilities{
	    .bSupported = false,
	    .Provider = ERhiPtlasProvider::VulkanNvPtlas,
	    .bNvidiaDeviceOnly = true,
	    .bCurrentDeviceIsNvidia = m_adapterInfo.VendorId == NvidiaVendorId,
	    .bSupportsDescriptorAccess = false,
	    .CapabilityStatusReason = m_adapterInfo.VendorId != NvidiaVendorId
	        ? "vulkan-nv-ptlas-requires-nvidia-device"
	        : (!m_featureStatus.RayTracing.bSupportsPtlasExtension
	                  ? "vulkan-nv-ptlas-extension-not-present"
	                  : (!m_featureStatus.RayTracing.bSupportsPtlasFeature ? "vulkan-nv-ptlas-feature-not-present" : "vulkan-ray-tracing-backend-not-enabled"))};

	m_rayTracingCapabilities.Groups.Provider = RhiRayTracingProviderCapabilities{.SelectedTlasProvider = ERhiRayTracingTlasProvider::None, .SelectedTlasProviderReason = "ray-tracing-not-enabled"};

	if (m_physicalDevice == VK_NULL_HANDLE || !m_featureStatus.RayTracing.bEnabledAccelerationStructure)
	{
		return;
	}

	VkPhysicalDeviceAccelerationStructurePropertiesKHR accelerationStructureProperties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR};
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR pipelineProperties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};
	VkPhysicalDeviceProperties2 properties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
	properties.pNext = &accelerationStructureProperties;
	if (m_featureStatus.RayTracing.bEnabledRayTracingPipeline)
	{
		pipelineProperties.pNext = properties.pNext;
		properties.pNext = &pipelineProperties;
	}
	vkGetPhysicalDeviceProperties2(m_physicalDevice, &properties);

	const bool bPipelineReadyTracingPipelines = m_featureStatus.RayTracing.bEnabledRayTracingPipeline && m_createRayTracingPipelines != nullptr;
	const bool bPipelineReadyGroupHandles = bPipelineReadyTracingPipelines && (m_getRayTracingShaderGroupHandles != nullptr);
	const bool bPipelineReadyTraceRays = bPipelineReadyGroupHandles && (m_cmdTraceRays != nullptr);
	const bool bPipelineReadyHandleSize = bPipelineReadyTraceRays && (pipelineProperties.shaderGroupHandleSize != 0);
	const bool bPipelineReadyBaseAlignment = bPipelineReadyHandleSize && (pipelineProperties.shaderGroupBaseAlignment != 0);
	const bool bPipelineReadyHandleAlignment = bPipelineReadyBaseAlignment && (pipelineProperties.shaderGroupHandleAlignment != 0);
	const bool bPipelineReady = bPipelineReadyHandleAlignment && (pipelineProperties.maxShaderGroupStride != 0);

	m_rayTracingCapabilities = RhiRayTracingCapabilities{
	    .bSupportsAccelerationStructure = true,
	    .bSupportsInlineRayQuery = m_featureStatus.RayTracing.bEnabledInlineRayQuery,
	    .bSupportsRayTracingPipeline = bPipelineReady,
	    .MaxTraceRecursionDepth = bPipelineReady ? pipelineProperties.maxRayRecursionDepth : 0,
	    .MaxRayPayloadSizeInBytes = bPipelineReady ? kRhiRayTracingMaxPayloadSizeInBytes : 0u,
	    .MaxRayAttributeSizeInBytes = bPipelineReady ? pipelineProperties.maxRayHitAttributeSize : 0,
	    .ShaderGroupHandleSizeInBytes = bPipelineReady ? pipelineProperties.shaderGroupHandleSize : 0,
	    .ShaderTableAlignmentInBytes = bPipelineReady ? pipelineProperties.shaderGroupBaseAlignment : 0,
	    .ShaderTableRecordAlignmentInBytes = bPipelineReady ? pipelineProperties.shaderGroupHandleAlignment : 0,
	    .MaxShaderTableRecordStrideInBytes = bPipelineReady ? pipelineProperties.maxShaderGroupStride : 0,
	    .AccelerationStructureByteAlignment = 256,
	    .ScratchBufferByteAlignment = accelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment,
	    .InstanceDescSizeInBytes = static_cast<std::uint32_t>(sizeof(VkAccelerationStructureInstanceKHR))};

	PopulateStandardRayTracingCapabilityGroups(m_rayTracingCapabilities);

	const bool bPtlasReadyBuildSizes = m_featureStatus.RayTracing.bEnabledPtlas && m_getPtlasBuildSizes != nullptr;
	const bool bPtlasReady = bPtlasReadyBuildSizes && (m_cmdBuildPtlas != nullptr);

	m_rayTracingCapabilities.Groups.Ptlas = RhiPtlasCapabilities{
	    .bSupported = bPtlasReady,
	    .Provider = ERhiPtlasProvider::VulkanNvPtlas,
	    .bNvidiaDeviceOnly = true,
	    .bCurrentDeviceIsNvidia = m_adapterInfo.VendorId == NvidiaVendorId,
	    .bSupportsDescriptorAccess = bPtlasReady,
	    .bSupportsCpuPackedOperations = m_featureStatus.RayTracing.bEnabledPtlas,
	    .bSupportsGpuDrivenOperations = m_featureStatus.RayTracing.bEnabledPtlas,
	    .bSupportsGpuOperationCount = m_featureStatus.RayTracing.bEnabledPtlas,
	    .bSupportsGpuWrittenInstanceRecords = m_featureStatus.RayTracing.bEnabledPtlas,
	    .bSupportsGpuWrittenPartitionRecords = m_featureStatus.RayTracing.bEnabledPtlas,
	    .bSupportsPartitionTranslation = m_featureStatus.RayTracing.bEnabledPtlas,
	    .bSupportsGlobalPartition = true,
	    .bSupportsExplicitInstanceAabb = true,
	    .MaxOperationsPerBuild = std::numeric_limits<std::uint32_t>::max(),
	    .InstanceWriteDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkPartitionedAccelerationStructureWriteInstanceDataNV)),
	    .InstanceUpdateDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkPartitionedAccelerationStructureUpdateInstanceDataNV)),
	    .PartitionWriteDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkPartitionedAccelerationStructureWritePartitionTranslationDataNV)),
	    .OperationDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkBuildPartitionedAccelerationStructureIndirectCommandNV)),
	    .OperationCountDataSizeInBytes = static_cast<std::uint32_t>(sizeof(std::uint32_t)),
	    .CapabilityStatusReason = m_adapterInfo.VendorId != NvidiaVendorId
	        ? "vulkan-nv-ptlas-requires-nvidia-device"
	        : (!m_featureStatus.RayTracing.bSupportsPtlasExtension
	                  ? "vulkan-nv-ptlas-extension-not-present"
	                  : (!m_featureStatus.RayTracing.bSupportsPtlasFeature
	                            ? "vulkan-nv-ptlas-feature-not-present"
	                            : (!m_featureStatus.RayTracing.bEnabledPtlas
	                                      ? "vulkan-nv-ptlas-not-enabled"
	                                      : (m_getPtlasBuildSizes == nullptr || m_cmdBuildPtlas == nullptr ? "vulkan-nv-ptlas-functions-not-loaded" : "vulkan-nv-ptlas-provider-ready"))))};

	SelectRayTracingTlasProvider();
}

void VulkanRhi::SelectRayTracingTlasProvider() noexcept
{
	RhiRayTracingProviderCapabilities& provider = m_rayTracingCapabilities.Groups.Provider;
	if (!m_rayTracingCapabilities.bSupportsAccelerationStructure)
	{
		provider = RhiRayTracingProviderCapabilities{.SelectedTlasProvider = ERhiRayTracingTlasProvider::None, .SelectedTlasProviderReason = "ray-tracing-unavailable"};
		return;
	}

	const RhiPtlasCapabilities& ptlas = m_rayTracingCapabilities.Groups.Ptlas;
	const bool bPtlasRequested = CVarRayTracingPreferPtlas.Get();
	const bool bPtlasSelectedSupported = bPtlasRequested && ptlas.bSupported;
	const bool bPtlasSelected = bPtlasSelectedSupported && (ptlas.bSupportsDescriptorAccess);
	const char* selectionReason = "TLAS-selected";
	if (bPtlasSelected)
	{
		selectionReason = "vulkan-nv-ptlas-selected";
	}
	else if (bPtlasRequested && ptlas.bSupported)
	{
		selectionReason = "vulkan-ptlas-descriptor-path-unavailable";
	}

	provider = RhiRayTracingProviderCapabilities{
	    .SelectedTlasProvider = bPtlasSelected ? ERhiRayTracingTlasProvider::Ptlas : ERhiRayTracingTlasProvider::Tlas,
	    .SelectedTlasProviderReason = selectionReason};
}
