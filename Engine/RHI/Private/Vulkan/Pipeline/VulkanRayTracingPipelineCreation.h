#pragma once

#include "RayTracing/RhiRayTracingPipelineDesc.h"
#include "Vulkan/VulkanIncludes.h"

#include <string>
#include <string_view>
#include <vector>

class VulkanRhi;

VkPipeline CreateVulkanRayTracingPipeline(VulkanRhi& rhi, const RayTracingPipelineDesc& desc, VkPipelineLayout pipelineLayout, std::string_view debugName, std::vector<std::string>& groupNames);
