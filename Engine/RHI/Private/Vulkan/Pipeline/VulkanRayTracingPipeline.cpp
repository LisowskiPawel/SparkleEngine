#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Pipeline/VulkanRayTracingPipeline.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Validation/RhiContract.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Diagnostics/VulkanDebugNames.h"
#include "Vulkan/Pipeline/VulkanPipelineLayoutBuilder.h"
#include "Vulkan/Pipeline/VulkanRayTracingPipelineCreation.h"

#include <limits>

VulkanRayTracingPipeline::VulkanRayTracingPipeline(VulkanRhi& rhi, const RayTracingPipelineDesc& desc) :
    RayTracingPipeline(desc),
    m_device(rhi.GetDevice())
{
	RhiContract::ValidateRayTracingPipelineDesc(desc);
	const RhiRayTracingCapabilities capabilities = rhi.GetRayTracingCapabilities();
	const bool bTracingPipelineTracingPipelines = !capabilities.bSupportsRayTracingPipeline || rhi.GetCreateRayTracingPipelines() == nullptr;
	const bool bUnsupportedTracingPipelineInBytes = bTracingPipelineTracingPipelines || (desc.MaxPayloadSizeInBytes > capabilities.MaxRayPayloadSizeInBytes);
	const bool bAttributeSizeUnsupported = bUnsupportedTracingPipelineInBytes || (desc.MaxAttributeSizeInBytes > capabilities.MaxRayAttributeSizeInBytes);
	const bool bUnsupportedRayTracingPipeline = bAttributeSizeUnsupported || (desc.MaxRecursionDepth > capabilities.MaxTraceRecursionDepth);

	if (bUnsupportedRayTracingPipeline)
	{
		throw Diagnostics::Error("Vulkan ray-tracing pipeline is not fully ready.");
	}
	const std::string debugName = desc.DebugName != nullptr ? Strings::ToNarrow(desc.DebugName) : "VulkanRayTracingPipeline";
	VulkanPipelineLayoutBuilder layoutBuilder;
	layoutBuilder.SetBindingLayout(desc.GlobalBindingLayout);
	m_pipelineLayout = layoutBuilder.Build(rhi, debugName);

	m_pipeline = CreateVulkanRayTracingPipeline(rhi, desc, GetPipelineLayout(), debugName, m_groupNames);
	VulkanDebugNames::SetObjectName(rhi.GetSetDebugUtilsObjectName(), m_device, VK_OBJECT_TYPE_PIPELINE, reinterpret_cast<std::uint64_t>(m_pipeline), debugName);
}

VulkanRayTracingPipeline::~VulkanRayTracingPipeline() noexcept
{
	if (m_device != VK_NULL_HANDLE && m_pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_device, m_pipeline, nullptr);
	}
}

VkPipelineLayout VulkanRayTracingPipeline::GetPipelineLayout() const noexcept
{
	return m_pipelineLayout != nullptr ? m_pipelineLayout->Get() : VK_NULL_HANDLE;
}

std::uint32_t VulkanRayTracingPipeline::FindShaderGroup(std::string_view exportName) const noexcept
{
	for (std::uint32_t index = 0; index < m_groupNames.size(); ++index)
	{
		if (m_groupNames[index] == exportName)
		{
			return index;
		}
	}
	return std::numeric_limits<std::uint32_t>::max();
}
