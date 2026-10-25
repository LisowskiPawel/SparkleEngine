#include "Vulkan/VulkanPCH.h"
#include "Vulkan/Device/VulkanCapabilities.h"

#include "Device/RhiCapabilitySupport.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Device/VulkanExternalFeatureInteropCapabilities.h"
#include "Vulkan/SwapChain/VulkanSwapChain.h"
#include "Vulkan/VulkanTypeConversions.h"
#include "Diagnostics/RhiDiagnostics.h"
#include "Shaders/ShaderTarget.h"

static RhiFormatSupport QueryFormatSupport(VulkanRhi& rhi, PixelFormat format) noexcept
{
	RhiFormatSupport support{.Format = format};
	if (format == PixelFormat::Unknown)
	{
		return support;
	}

	const VkFormat nativeFormat = VulkanTypeConversions::ToVkFormat(format);
	if (nativeFormat == VK_FORMAT_UNDEFINED)
	{
		return support;
	}

	VkFormatProperties properties{};
	vkGetPhysicalDeviceFormatProperties(rhi.GetPhysicalDevice(), nativeFormat, &properties);
	const VkFormatFeatureFlags optimal = properties.optimalTilingFeatures;

	support.SupportsTexture = (optimal & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0 || (optimal & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) != 0
	    || (optimal & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;

	support.SupportsShaderResource = (optimal & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
	support.SupportsUnorderedAccess = (optimal & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;
	support.SupportsRenderTarget = (optimal & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) != 0;
	support.SupportsDepthStencil = (optimal & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
	return support;
}

RhiCapabilities BuildVulkanCapabilities(VulkanRhi& rhi, VulkanSwapChain& swapChain, RenderDiagnostics& diagnostics, bool hasCommandRecordingContext) noexcept
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(rhi.GetPhysicalDevice(), &properties);

	RhiCapabilities capabilities{};
	capabilities.BackendApi = ERhiBackendApi::Vulkan;
	capabilities.RuntimeShaderBinaryFormat = ShaderBinaryFormat::SpirV;
	const std::uint32_t apiVersion = rhi.GetAdapterInfo().ApiVersion;

	capabilities.BackendVersion = RhiBackendVersionInfo{
	    .Semantic = ERhiBackendVersionSemantic::ApiVersion,
	    .Major = VK_VERSION_MAJOR(apiVersion),
	    .Minor = VK_VERSION_MINOR(apiVersion),
	    .Patch = VK_VERSION_PATCH(apiVersion),
	    .PackedValue = apiVersion};

	capabilities.DescriptorModel = ERhiDescriptorModel::DescriptorSets;

	capabilities.BindingLimits = RhiBindingLimits{
	    .MaxDescriptorSets = properties.limits.maxBoundDescriptorSets,
	    .MaxShaderResourceDescriptors = properties.limits.maxDescriptorSetSampledImages + properties.limits.maxDescriptorSetStorageImages + properties.limits.maxDescriptorSetUniformBuffers
	        + properties.limits.maxDescriptorSetStorageBuffers,
	    .MaxSamplerDescriptors = properties.limits.maxDescriptorSetSamplers,
	    .MaxDescriptorTableEntries = properties.limits.maxDescriptorSetSampledImages + properties.limits.maxDescriptorSetStorageImages,
	    .MaxPushConstantBytes = properties.limits.maxPushConstantsSize};

	capabilities.DescriptorIndexing = RhiDescriptorIndexingCapabilities{
	    .SupportsSampledImageArrayNonUniformIndexing = rhi.GetFeatureStatus().EnabledSampledImageArrayNonUniformIndexing,
	    .SupportsPartiallyBoundDescriptorArrays = rhi.GetFeatureStatus().EnabledPartiallyBoundDescriptorArrays};

	capabilities.UploadReadback = RhiUploadReadbackCapabilities{.SupportsBufferUpload = true, .SupportsTextureUpload = true, .SupportsReadback = true};

	capabilities.Presentation = RhiPresentationCapabilities{
	    .BackBufferCount = swapChain.GetBackBufferCount(),
	    .MaximumFramesInFlight = swapChain.GetMaximumFramesInFlight(),
	    .Throttle = ERhiPresentationThrottle::SwapChainImageAcquisition};

	for (std::size_t index = 0; index < capabilities.FormatSupport.size(); ++index)
	{
		capabilities.FormatSupport[index] = QueryFormatSupport(rhi, kRhiCapabilityPixelFormats[index]);
	}
	capabilities.Diagnostics = BuildRhiBackendDiagnosticsSupport(rhi.IsValidationEnabled(), diagnostics);
	capabilities.RayTracing = rhi.GetRayTracingCapabilities();
	capabilities.SupportsShaderFloat64 = rhi.GetFeatureStatus().EnabledShaderFloat64;
	capabilities.SupportsMeshShaders = false;
	capabilities.SupportsTaskShaders = false;
	const bool hasComputeQueue = rhi.GetQueue(ERhiQueueType::Compute) != VK_NULL_HANDLE;
	const bool hasCopyQueue = rhi.GetQueue(ERhiQueueType::Copy) != VK_NULL_HANDLE;
	capabilities.Queues.Set(ERhiQueueType::Graphics, true, true);
	capabilities.Queues.Set(ERhiQueueType::Compute, hasComputeQueue, hasComputeQueue && rhi.HasIndependentQueue(ERhiQueueType::Compute));
	capabilities.Queues.Set(ERhiQueueType::Copy, hasCopyQueue, hasCopyQueue && rhi.HasIndependentQueue(ERhiQueueType::Copy));
	capabilities.SupportsPresent = swapChain.GetBackBufferFormat() != PixelFormat::Unknown;
	capabilities.MemoryAllocator = ERhiMemoryAllocatorBackend::VulkanManaged;
	capabilities.MemorySupport = BuildRhiBackendMemorySupport(diagnostics);
	capabilities.ExternalFeatureInterop = BuildVulkanExternalFeatureInteropCapabilities(&rhi, hasCommandRecordingContext);
	return capabilities;
}
