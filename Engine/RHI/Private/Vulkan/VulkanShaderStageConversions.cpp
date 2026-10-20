#include "Vulkan/VulkanPCH.h"

#include "Vulkan/VulkanTypeConversions.h"

VkShaderStageFlagBits VulkanTypeConversions::ToVkShaderStage(ShaderStage stage) noexcept
{
	switch (stage)
	{
		case ShaderStage::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderStage::Pixel:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderStage::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		case ShaderStage::Geometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderStage::Hull:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderStage::Domain:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case ShaderStage::RayGeneration:
			return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case ShaderStage::Miss:
			return VK_SHADER_STAGE_MISS_BIT_KHR;
		case ShaderStage::ClosestHit:
			return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case ShaderStage::AnyHit:
			return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case ShaderStage::Intersection:
			return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case ShaderStage::Callable:
			return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		case ShaderStage::Count:
		default:
			return VK_SHADER_STAGE_ALL;
	}
}

VkShaderStageFlags VulkanTypeConversions::ToVkShaderStages(ShaderStageMask visibilityMask) noexcept
{
	VkShaderStageFlags result = 0;
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::Vertex))
	{
		result |= VK_SHADER_STAGE_VERTEX_BIT;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::Pixel))
	{
		result |= VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::Compute))
	{
		result |= VK_SHADER_STAGE_COMPUTE_BIT;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::RayGeneration))
	{
		result |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::Miss))
	{
		result |= VK_SHADER_STAGE_MISS_BIT_KHR;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::ClosestHit))
	{
		result |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::AnyHit))
	{
		result |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::Intersection))
	{
		result |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
	}
	if (HasAnyShaderStageMask(visibilityMask, ShaderStageMask::Callable))
	{
		result |= VK_SHADER_STAGE_CALLABLE_BIT_KHR;
	}
	return result != 0 ? result : VK_SHADER_STAGE_ALL;
}
