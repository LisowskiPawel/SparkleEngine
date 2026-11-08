#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Pipeline/VulkanRayTracingPipelineCreation.h"
#include "Vulkan/Pipeline/VulkanShaderModule.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Core/VulkanResult.h"
#include "Core/Public/Diagnostics/Error.h"

#include <span>
#include <unordered_map>

class VulkanRayTracingPipelineAssembly final
{
public:
	VulkanRayTracingPipelineAssembly(VulkanRhi& rhi, const RayTracingPipelineDesc& desc, std::string_view debugName, std::vector<std::string>& groupNames)
	{
		BuildShaderStages(rhi, desc, debugName);
		BuildGeneralGroups(desc, groupNames);
		BuildHitGroups(desc, groupNames);
	}

	VulkanRayTracingPipelineAssembly(const VulkanRayTracingPipelineAssembly&) = delete;
	VulkanRayTracingPipelineAssembly& operator=(const VulkanRayTracingPipelineAssembly&) = delete;
	VulkanRayTracingPipelineAssembly(VulkanRayTracingPipelineAssembly&&) = delete;
	VulkanRayTracingPipelineAssembly& operator=(VulkanRayTracingPipelineAssembly&&) = delete;

	std::span<const VkPipelineShaderStageCreateInfo> GetStages() const noexcept { return m_stages; }

	std::span<const VkRayTracingShaderGroupCreateInfoKHR> GetGroups() const noexcept { return m_groups; }

private:
	void BuildShaderStages(VulkanRhi& rhi, const RayTracingPipelineDesc& desc, std::string_view debugName)
	{
		m_modules.reserve(desc.ShaderExports.size());
		m_stages.reserve(desc.ShaderExports.size());
		for (const RhiRayTracingShaderExportDesc& shaderExport : desc.ShaderExports)
		{
			m_modules.emplace_back(rhi, RhiShaderStageDesc{shaderExport.Shader}, debugName);
			m_stages.push_back(m_modules.back().BuildStageCreateInfo());
			m_stageIndices.emplace(shaderExport.ExportName, static_cast<std::uint32_t>(m_stages.size() - 1));
		}
	}

	void BuildGeneralGroups(const RayTracingPipelineDesc& desc, std::vector<std::string>& groupNames)
	{
		m_groups.reserve(desc.ShaderExports.size() + desc.HitGroups.size());
		for (std::uint32_t index = 0; index < desc.ShaderExports.size(); ++index)
		{
			const RhiRayTracingShaderExportDesc& shaderExport = desc.ShaderExports[index];
			const ShaderStage stage = shaderExport.Shader->Entry->Stage;
			if (stage != ShaderStage::RayGeneration && stage != ShaderStage::Miss && stage != ShaderStage::Callable)
			{
				continue;
			}

			m_groups.push_back(
			    VkRayTracingShaderGroupCreateInfoKHR{
			        .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
			        .pNext = nullptr,
			        .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
			        .generalShader = index,
			        .closestHitShader = VK_SHADER_UNUSED_KHR,
			        .anyHitShader = VK_SHADER_UNUSED_KHR,
			        .intersectionShader = VK_SHADER_UNUSED_KHR,
			        .pShaderGroupCaptureReplayHandle = nullptr});

			groupNames.emplace_back(shaderExport.ExportName);
		}
	}

	void BuildHitGroups(const RayTracingPipelineDesc& desc, std::vector<std::string>& groupNames)
	{
		const auto stageIndex = [this](std::string_view name)
		{
			const auto found = m_stageIndices.find(name);
			return found != m_stageIndices.end() ? found->second : VK_SHADER_UNUSED_KHR;
		};

		for (const RhiRayTracingHitGroupDesc& hitGroup : desc.HitGroups)
		{
			m_groups.push_back(
			    VkRayTracingShaderGroupCreateInfoKHR{
			        .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
			        .pNext = nullptr,
			        .type = hitGroup.Kind == ERhiRayTracingHitGroupKind::Triangles ? VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR
			                                                                       : VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR,
			        .generalShader = VK_SHADER_UNUSED_KHR,
			        .closestHitShader = stageIndex(hitGroup.ClosestHitExport),
			        .anyHitShader = stageIndex(hitGroup.AnyHitExport),
			        .intersectionShader = stageIndex(hitGroup.IntersectionExport),
			        .pShaderGroupCaptureReplayHandle = nullptr});

			groupNames.emplace_back(hitGroup.ExportName);
		}
	}

	std::vector<VulkanShaderModule> m_modules;
	std::vector<VkPipelineShaderStageCreateInfo> m_stages;
	std::unordered_map<std::string_view, std::uint32_t> m_stageIndices;
	std::vector<VkRayTracingShaderGroupCreateInfoKHR> m_groups;
};

VkPipeline CreateVulkanRayTracingPipeline(VulkanRhi& rhi, const RayTracingPipelineDesc& desc, VkPipelineLayout pipelineLayout, std::string_view debugName, std::vector<std::string>& groupNames)
{
	VulkanRayTracingPipelineAssembly assembly(rhi, desc, debugName, groupNames);
	VkPipeline pipeline = VK_NULL_HANDLE;

	const VkRayTracingPipelineCreateInfoKHR createInfo{
	    .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
	    .pNext = nullptr,
	    .flags = 0,
	    .stageCount = static_cast<std::uint32_t>(assembly.GetStages().size()),
	    .pStages = assembly.GetStages().data(),
	    .groupCount = static_cast<std::uint32_t>(assembly.GetGroups().size()),
	    .pGroups = assembly.GetGroups().data(),
	    .maxPipelineRayRecursionDepth = desc.MaxRecursionDepth,
	    .pLibraryInfo = nullptr,
	    .pLibraryInterface = nullptr,
	    .pDynamicState = nullptr,
	    .layout = pipelineLayout,
	    .basePipelineHandle = VK_NULL_HANDLE,
	    .basePipelineIndex = -1};

	const VkResult result = rhi.GetCreateRayTracingPipelines()(rhi.GetDevice(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
	if (!VulkanResult::Succeeded(result))
	{
		throw Diagnostics::Error(VulkanResult::FormatFailure("vkCreateRayTracingPipelinesKHR", result));
	}
	return pipeline;
}
