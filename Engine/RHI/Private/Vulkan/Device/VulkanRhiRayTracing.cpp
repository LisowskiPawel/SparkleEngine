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
	if (m_device == VK_NULL_HANDLE || !m_featureStatus.RayTracing.EnabledAccelerationStructure)
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
	if (m_featureStatus.RayTracing.EnabledRayTracingPipeline)
	{
		m_createRayTracingPipelines = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(m_device, "vkCreateRayTracingPipelinesKHR"));
		m_getRayTracingShaderGroupHandles = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(m_device, "vkGetRayTracingShaderGroupHandlesKHR"));
		m_cmdTraceRays = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(m_device, "vkCmdTraceRaysKHR"));
	}
	if (m_featureStatus.RayTracing.EnabledPtlas)
	{
		m_getPtlasBuildSizes = reinterpret_cast<PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV>(vkGetDeviceProcAddr(m_device, "vkGetPartitionedAccelerationStructuresBuildSizesNV"));

		m_cmdBuildPtlas = reinterpret_cast<PFN_vkCmdBuildPartitionedAccelerationStructuresNV>(vkGetDeviceProcAddr(m_device, "vkCmdBuildPartitionedAccelerationStructuresNV"));
	}

	const bool loaded = m_getBufferDeviceAddress != nullptr && m_createAccelerationStructure != nullptr && m_destroyAccelerationStructure != nullptr && m_getAccelerationStructureBuildSizes != nullptr
	    && m_cmdBuildAccelerationStructures != nullptr && m_getAccelerationStructureDeviceAddress != nullptr;

	if (!loaded)
	{
		m_featureStatus.RayTracing.EnabledAccelerationStructure = false;
		m_featureStatus.RayTracing.EnabledInlineRayQuery = false;
		m_featureStatus.RayTracing.EnabledRayTracingPipeline = false;
		m_featureStatus.RayTracing.EnabledPtlas = false;
	}
	if (m_createRayTracingPipelines == nullptr || m_getRayTracingShaderGroupHandles == nullptr || m_cmdTraceRays == nullptr)
	{
		m_featureStatus.RayTracing.EnabledRayTracingPipeline = false;
	}
	if (m_featureStatus.RayTracing.EnabledPtlas && (m_getPtlasBuildSizes == nullptr || m_cmdBuildPtlas == nullptr))
	{
		m_featureStatus.RayTracing.EnabledPtlas = false;
	}
}

void VulkanRhi::BuildRayTracingCapabilities() noexcept
{
	m_rayTracingCapabilities = {};

	m_rayTracingCapabilities.Groups.Ptlas = RhiPtlasCapabilities{
	    .Supported = false,
	    .Provider = ERhiPtlasProvider::VulkanNvPtlas,
	    .NvidiaDeviceOnly = true,
	    .CurrentDeviceIsNvidia = m_adapterInfo.VendorId == NvidiaVendorId,
	    .SupportsDescriptorAccess = false,
	    .CapabilityStatusReason = m_adapterInfo.VendorId != NvidiaVendorId
	        ? "vulkan-nv-ptlas-requires-nvidia-device"
	        : (!m_featureStatus.RayTracing.SupportsPtlasExtension
	                  ? "vulkan-nv-ptlas-extension-not-present"
	                  : (!m_featureStatus.RayTracing.SupportsPtlasFeature ? "vulkan-nv-ptlas-feature-not-present" : "vulkan-ray-tracing-backend-not-enabled"))};

	m_rayTracingCapabilities.Groups.Provider = RhiRayTracingProviderCapabilities{.SelectedTlasProvider = ERhiRayTracingTlasProvider::None, .SelectedTlasProviderReason = "ray-tracing-not-enabled"};

	if (m_physicalDevice == VK_NULL_HANDLE || !m_featureStatus.RayTracing.EnabledAccelerationStructure)
	{
		return;
	}

	VkPhysicalDeviceAccelerationStructurePropertiesKHR accelerationStructureProperties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR};
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR pipelineProperties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};
	VkPhysicalDeviceProperties2 properties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
	properties.pNext = &accelerationStructureProperties;
	if (m_featureStatus.RayTracing.EnabledRayTracingPipeline)
	{
		pipelineProperties.pNext = properties.pNext;
		properties.pNext = &pipelineProperties;
	}
	vkGetPhysicalDeviceProperties2(m_physicalDevice, &properties);

	const bool pipelineReady = m_featureStatus.RayTracing.EnabledRayTracingPipeline && m_createRayTracingPipelines != nullptr && m_getRayTracingShaderGroupHandles != nullptr
	    && m_cmdTraceRays != nullptr && pipelineProperties.shaderGroupHandleSize != 0 && pipelineProperties.shaderGroupBaseAlignment != 0 && pipelineProperties.shaderGroupHandleAlignment != 0
	    && pipelineProperties.maxShaderGroupStride != 0;

	m_rayTracingCapabilities = RhiRayTracingCapabilities{
	    .SupportsAccelerationStructure = true,
	    .SupportsInlineRayQuery = m_featureStatus.RayTracing.EnabledInlineRayQuery,
	    .SupportsRayTracingPipeline = pipelineReady,
	    .MaxTraceRecursionDepth = pipelineReady ? pipelineProperties.maxRayRecursionDepth : 0,
	    .MaxRayPayloadSizeInBytes = pipelineReady ? kRhiRayTracingMaxPayloadSizeInBytes : 0u,
	    .MaxRayAttributeSizeInBytes = pipelineReady ? pipelineProperties.maxRayHitAttributeSize : 0,
	    .ShaderGroupHandleSizeInBytes = pipelineReady ? pipelineProperties.shaderGroupHandleSize : 0,
	    .ShaderTableAlignmentInBytes = pipelineReady ? pipelineProperties.shaderGroupBaseAlignment : 0,
	    .ShaderTableRecordAlignmentInBytes = pipelineReady ? pipelineProperties.shaderGroupHandleAlignment : 0,
	    .MaxShaderTableRecordStrideInBytes = pipelineReady ? pipelineProperties.maxShaderGroupStride : 0,
	    .AccelerationStructureByteAlignment = 256,
	    .ScratchBufferByteAlignment = accelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment,
	    .InstanceDescSizeInBytes = static_cast<std::uint32_t>(sizeof(VkAccelerationStructureInstanceKHR))};

	PopulateStandardRayTracingCapabilityGroups(m_rayTracingCapabilities);

	const bool ptlasReady = m_featureStatus.RayTracing.EnabledPtlas && m_getPtlasBuildSizes != nullptr && m_cmdBuildPtlas != nullptr;

	m_rayTracingCapabilities.Groups.Ptlas = RhiPtlasCapabilities{
	    .Supported = ptlasReady,
	    .Provider = ERhiPtlasProvider::VulkanNvPtlas,
	    .NvidiaDeviceOnly = true,
	    .CurrentDeviceIsNvidia = m_adapterInfo.VendorId == NvidiaVendorId,
	    .SupportsDescriptorAccess = ptlasReady,
	    .SupportsCpuPackedOperations = m_featureStatus.RayTracing.EnabledPtlas,
	    .SupportsGpuDrivenOperations = m_featureStatus.RayTracing.EnabledPtlas,
	    .SupportsGpuOperationCount = m_featureStatus.RayTracing.EnabledPtlas,
	    .SupportsGpuWrittenInstanceRecords = m_featureStatus.RayTracing.EnabledPtlas,
	    .SupportsGpuWrittenPartitionRecords = m_featureStatus.RayTracing.EnabledPtlas,
	    .SupportsPartitionTranslation = m_featureStatus.RayTracing.EnabledPtlas,
	    .SupportsGlobalPartition = true,
	    .SupportsExplicitInstanceAabb = true,
	    .MaxOperationsPerBuild = std::numeric_limits<std::uint32_t>::max(),
	    .InstanceWriteDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkPartitionedAccelerationStructureWriteInstanceDataNV)),
	    .InstanceUpdateDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkPartitionedAccelerationStructureUpdateInstanceDataNV)),
	    .PartitionWriteDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkPartitionedAccelerationStructureWritePartitionTranslationDataNV)),
	    .OperationDataSizeInBytes = static_cast<std::uint32_t>(sizeof(VkBuildPartitionedAccelerationStructureIndirectCommandNV)),
	    .OperationCountDataSizeInBytes = static_cast<std::uint32_t>(sizeof(std::uint32_t)),
	    .CapabilityStatusReason = m_adapterInfo.VendorId != NvidiaVendorId
	        ? "vulkan-nv-ptlas-requires-nvidia-device"
	        : (!m_featureStatus.RayTracing.SupportsPtlasExtension
	                  ? "vulkan-nv-ptlas-extension-not-present"
	                  : (!m_featureStatus.RayTracing.SupportsPtlasFeature
	                            ? "vulkan-nv-ptlas-feature-not-present"
	                            : (!m_featureStatus.RayTracing.EnabledPtlas
	                                      ? "vulkan-nv-ptlas-not-enabled"
	                                      : (m_getPtlasBuildSizes == nullptr || m_cmdBuildPtlas == nullptr ? "vulkan-nv-ptlas-functions-not-loaded" : "vulkan-nv-ptlas-provider-ready"))))};

	SelectRayTracingTlasProvider();
}

void VulkanRhi::SelectRayTracingTlasProvider() noexcept
{
	RhiRayTracingProviderCapabilities& provider = m_rayTracingCapabilities.Groups.Provider;
	if (!m_rayTracingCapabilities.SupportsAccelerationStructure)
	{
		provider = RhiRayTracingProviderCapabilities{.SelectedTlasProvider = ERhiRayTracingTlasProvider::None, .SelectedTlasProviderReason = "ray-tracing-unavailable"};
		return;
	}

	const RhiPtlasCapabilities& ptlas = m_rayTracingCapabilities.Groups.Ptlas;
	const bool ptlasRequested = CVarRayTracingPreferPtlas.Get();
	const bool ptlasSelected = ptlasRequested && ptlas.Supported && ptlas.SupportsDescriptorAccess;
	const char* selectionReason = "TLAS-selected";
	if (ptlasSelected)
	{
		selectionReason = "vulkan-nv-ptlas-selected";
	}
	else if (ptlasRequested && ptlas.Supported)
	{
		selectionReason = "vulkan-ptlas-descriptor-path-unavailable";
	}

	provider = RhiRayTracingProviderCapabilities{
	    .SelectedTlasProvider = ptlasSelected ? ERhiRayTracingTlasProvider::Ptlas : ERhiRayTracingTlasProvider::Tlas,
	    .SelectedTlasProviderReason = selectionReason};
}
