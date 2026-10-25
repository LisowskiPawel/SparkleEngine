#include "Vulkan/VulkanPCH.h"

#include "Vulkan/VulkanRenderHardwareInterface.h"

#include "Shaders/ShaderTarget.h"
#include "Vulkan/Capture/VulkanCaptureService.h"
#include "Vulkan/Commands/VulkanCommandRecordingContext.h"
#include "Vulkan/Commands/VulkanRenderCommandList.h"
#include "Vulkan/Descriptors/VulkanDescriptorAllocator.h"
#include "Vulkan/Descriptors/VulkanDescriptorService.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Device/VulkanExternalFeatureInteropCapabilities.h"
#include "Vulkan/Diagnostics/VulkanRenderDiagnostics.h"
#include "Vulkan/Interop/VulkanInteropService.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"
#include "Vulkan/Pipeline/VulkanBindingLayout.h"
#include "Vulkan/Pipeline/VulkanPipeline.h"
#include "Vulkan/Pipeline/VulkanRayTracingPipeline.h"
#include "Presentation/RhiPresentationServiceAdapter.h"
#include "Vulkan/SwapChain/VulkanBackBufferPresentation.h"
#include "Pipeline/RhiPipelineServiceAdapter.h"
#include "Vulkan/RayTracing/VulkanRayTracingServices.h"
#include "Vulkan/Resources/VulkanResourceService.h"
#include "Vulkan/Resources/VulkanUploadService.h"
#include "Vulkan/Samplers/VulkanSamplerLibrary.h"
#include "Vulkan/SwapChain/VulkanSwapChain.h"
#include "Vulkan/UI/VulkanImGuiBackend.h"
#include "Vulkan/VulkanTypeConversions.h"

#include <array>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanRenderHardwareInterfaceLogger, "RHI.Vulkan.Interface");

VulkanRenderHardwareInterface::VulkanRenderHardwareInterface(VulkanRhi& rhi, VulkanSwapChain& swapChain, VulkanGpuMemoryAllocator& memoryAllocator) noexcept :
    m_rhi(&rhi),
    m_swapChain(&swapChain),
    m_memoryAllocator(&memoryAllocator)
{
	m_interopService = std::make_unique<VulkanInteropService>(*this);
	m_captureService = std::make_unique<VulkanCaptureService>(rhi);
	m_pipelineService = std::make_unique<RhiPipelineServiceAdapter<VulkanRhi, VulkanPipeline, VulkanRayTracingPipeline, VulkanBindingLayoutCompiler>>(rhi);
	m_rayTracingServices = std::make_unique<VulkanRayTracingServices>(rhi, memoryAllocator);
	m_descriptorService = std::make_unique<VulkanDescriptorService>(rhi, memoryAllocator, m_capabilities);
	m_resourceService = std::make_unique<VulkanResourceService>(rhi, memoryAllocator, m_capabilities);
	m_presentationService = std::make_unique<RhiPresentationServiceAdapter<VulkanBackBufferPresentation>>(swapChain, *m_descriptorService, *m_resourceService, m_currentFrameIndex);
	m_uploadService = std::make_unique<VulkanUploadService>(memoryAllocator);
	m_samplerLibrary = std::make_unique<VulkanSamplerLibrary>(rhi, *m_descriptorService);
	m_descriptorService->SetSamplerLibrary(*m_samplerLibrary);
	m_imguiBackend = std::make_unique<VulkanImGuiBackend>(*this, *m_descriptorService);
	m_diagnostics = CreateVulkanRenderDiagnostics(rhi, memoryAllocator);
	RebuildSwapChainBackBufferViews();
	m_capabilities = BuildCapabilities();
}

VulkanRenderHardwareInterface::~VulkanRenderHardwareInterface() noexcept
{
	m_samplerLibrary.reset();
	m_imguiBackend.reset();
	m_resourceService.reset();
	m_uploadService.reset();
}

std::uint32_t VulkanRenderHardwareInterface::GetCurrentFrameIndex() const noexcept
{
	return m_currentFrameIndex;
}

RhiResourceService& VulkanRenderHardwareInterface::GetResourceService() noexcept
{
	return *m_resourceService;
}

const RhiResourceService& VulkanRenderHardwareInterface::GetResourceService() const noexcept
{
	return *m_resourceService;
}

RhiDescriptorService& VulkanRenderHardwareInterface::GetDescriptorService() noexcept
{
	return *m_descriptorService;
}

const RhiDescriptorService& VulkanRenderHardwareInterface::GetDescriptorService() const noexcept
{
	return *m_descriptorService;
}

RhiPipelineService& VulkanRenderHardwareInterface::GetPipelineService() noexcept
{
	return *m_pipelineService;
}

RhiUploadService& VulkanRenderHardwareInterface::GetUploadService() noexcept
{
	return *m_uploadService;
}

const RhiUploadService& VulkanRenderHardwareInterface::GetUploadService() const noexcept
{
	return *m_uploadService;
}

RhiRayTracingService& VulkanRenderHardwareInterface::GetRayTracingService() noexcept
{
	return *m_rayTracingServices;
}

const RhiRayTracingService& VulkanRenderHardwareInterface::GetRayTracingService() const noexcept
{
	return *m_rayTracingServices;
}

void VulkanRenderHardwareInterface::WaitForIdle() noexcept
{
	m_rhi->WaitForIdle();
	m_resourceService->FlushDeferredResourceReleases();
}

RhiInteropService& VulkanRenderHardwareInterface::GetInteropService() noexcept
{
	return *m_interopService;
}

const RhiInteropService& VulkanRenderHardwareInterface::GetInteropService() const noexcept
{
	return *m_interopService;
}

RhiCaptureService& VulkanRenderHardwareInterface::GetCaptureService() noexcept
{
	return *m_captureService;
}

RhiPresentationService& VulkanRenderHardwareInterface::GetPresentationService() noexcept
{
	return *m_presentationService;
}

const RhiPresentationService& VulkanRenderHardwareInterface::GetPresentationService() const noexcept
{
	return *m_presentationService;
}

NativeGraphicsDeviceHandle VulkanRenderHardwareInterface::GetDeviceHandle() const noexcept
{
	return NativeGraphicsDeviceHandle{m_rhi->GetDevice()};
}

NativeGraphicsQueueHandle VulkanRenderHardwareInterface::GetGraphicsQueueHandle() const noexcept
{
	return NativeGraphicsQueueHandle{m_rhi->GetGraphicsQueue()};
}

RenderCommandList& VulkanRenderHardwareInterface::GetGraphicsCommandList(std::uint32_t) noexcept
{
	return GetCommandList(ERhiQueueType::Graphics, m_currentFrameIndex);
}

RenderCommandList& VulkanRenderHardwareInterface::GetCommandList(ERhiQueueType queueType, std::uint32_t frameIndex) noexcept
{
	m_resourceService->DrainCompletedResourceReleases();
	return m_commandRecordingContext->GetCurrentCommandList(queueType, frameIndex);
}

RhiRayTracingCapabilities VulkanRenderHardwareInterface::GetRayTracingCapabilities() const noexcept
{
	return m_rayTracingServices->GetCapabilities();
}

RhiCapabilities VulkanRenderHardwareInterface::BuildCapabilities() const noexcept
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(m_rhi->GetPhysicalDevice(), &properties);

	RhiCapabilities capabilities{};
	capabilities.BackendApi = ERhiBackendApi::Vulkan;
	capabilities.RuntimeShaderBinaryFormat = ShaderBinaryFormat::SpirV;
	const std::uint32_t apiVersion = m_rhi->GetAdapterInfo().ApiVersion;

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
	    .SupportsSampledImageArrayNonUniformIndexing = m_rhi->GetFeatureStatus().EnabledSampledImageArrayNonUniformIndexing,
	    .SupportsPartiallyBoundDescriptorArrays = m_rhi->GetFeatureStatus().EnabledPartiallyBoundDescriptorArrays};

	capabilities.UploadReadback = RhiUploadReadbackCapabilities{.SupportsBufferUpload = true, .SupportsTextureUpload = true, .SupportsReadback = true};

	capabilities.Presentation = RhiPresentationCapabilities{
	    .BackBufferCount = m_swapChain->GetBackBufferCount(),
	    .MaximumFramesInFlight = m_swapChain->GetMaximumFramesInFlight(),
	    .Throttle = ERhiPresentationThrottle::SwapChainImageAcquisition};

	for (std::size_t index = 0; index < capabilities.FormatSupport.size(); ++index)
	{
		capabilities.FormatSupport[index] = QueryFormatSupport(kRhiCapabilityPixelFormats[index]);
	}
	capabilities.Diagnostics = BuildBackendDiagnosticsSupport();
	capabilities.RayTracing = m_rhi->GetRayTracingCapabilities();
	capabilities.SupportsShaderFloat64 = m_rhi->GetFeatureStatus().EnabledShaderFloat64;
	capabilities.SupportsMeshShaders = false;
	capabilities.SupportsTaskShaders = false;
	const bool hasComputeQueue = m_rhi->GetQueue(ERhiQueueType::Compute) != VK_NULL_HANDLE;
	const bool hasCopyQueue = m_rhi->GetQueue(ERhiQueueType::Copy) != VK_NULL_HANDLE;
	capabilities.Queues.Set(ERhiQueueType::Graphics, true, true);
	capabilities.Queues.Set(ERhiQueueType::Compute, hasComputeQueue, hasComputeQueue && m_rhi->HasIndependentQueue(ERhiQueueType::Compute));
	capabilities.Queues.Set(ERhiQueueType::Copy, hasCopyQueue, hasCopyQueue && m_rhi->HasIndependentQueue(ERhiQueueType::Copy));
	capabilities.SupportsPresent = m_swapChain->GetBackBufferFormat() != PixelFormat::Unknown;
	capabilities.MemoryAllocator = ERhiMemoryAllocatorBackend::VulkanManaged;
	capabilities.MemorySupport = BuildBackendMemorySupport();
	capabilities.ExternalFeatureInterop = BuildVulkanExternalFeatureInteropCapabilities(m_rhi, m_commandRecordingContext != nullptr);
	return capabilities;
}

RhiBackendDiagnosticsSupport VulkanRenderHardwareInterface::BuildBackendDiagnosticsSupport() const noexcept
{
	const bool validationEnabled = m_rhi->IsValidationEnabled();
	const RhiDiagnosticsCapabilities diagnostics = m_diagnostics->GetCapabilities();

	return RhiBackendDiagnosticsSupport{
	    .ValidationEnabled = validationEnabled,
	    .SupportsDebugLayer = validationEnabled,
	    .SupportsObjectNames = diagnostics.SupportsObjectNames,
	    .SupportsGpuEvents = diagnostics.SupportsGpuEvents,
	    .SupportsTimestampQueries = diagnostics.SupportsTimestampQueries,
	    .SupportsDebugMessages = diagnostics.SupportsDebugMessages,
	    .SupportsLiveObjectReports = diagnostics.SupportsLiveObjectReports,
	    .SupportsCrashDiagnostics = diagnostics.SupportsCrashDiagnostics};
}

RhiBackendMemorySupport VulkanRenderHardwareInterface::BuildBackendMemorySupport() const noexcept
{
	const RenderMemoryDiagnostics* const diagnostics = m_diagnostics->GetMemoryDiagnostics();

	return RhiBackendMemorySupport{
	    .SupportsMemoryDiagnostics = diagnostics != nullptr,
	    .SupportsBudgetQueries = diagnostics != nullptr && diagnostics->SupportsBudgetQueries(),
	    .SupportsDelayedDestructionTracking = diagnostics != nullptr && diagnostics->SupportsDelayedDestructionTracking(),
	    .SupportsResidencyPressure = diagnostics != nullptr && diagnostics->SupportsBudgetQueries()};
}

RhiFormatSupport VulkanRenderHardwareInterface::QueryFormatSupport(PixelFormat format) const noexcept
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
	vkGetPhysicalDeviceFormatProperties(m_rhi->GetPhysicalDevice(), nativeFormat, &properties);
	const VkFormatFeatureFlags optimal = properties.optimalTilingFeatures;

	support.SupportsTexture = (optimal & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0 || (optimal & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) != 0
	    || (optimal & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;

	support.SupportsShaderResource = (optimal & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
	support.SupportsUnorderedAccess = (optimal & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;
	support.SupportsRenderTarget = (optimal & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) != 0;
	support.SupportsDepthStencil = (optimal & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
	return support;
}

RenderDiagnostics& VulkanRenderHardwareInterface::GetDiagnostics() noexcept
{
	return *m_diagnostics;
}

const RenderDiagnostics& VulkanRenderHardwareInterface::GetDiagnostics() const noexcept
{
	return *m_diagnostics;
}

RhiImGuiRenderer& VulkanRenderHardwareInterface::GetImGuiRenderer() noexcept
{
	return *m_imguiBackend;
}

RhiRayTracingAccelerationStructurePrebuildInfo VulkanRenderHardwareInterface::GetBottomLevelAccelerationStructurePrebuildInfo(const RhiRayTracingGeometryDesc& geometry) const noexcept
{
	return m_rayTracingServices->GetBottomLevelAccelerationStructurePrebuildInfo(geometry);
}

RhiRayTracingAccelerationStructurePrebuildInfo VulkanRenderHardwareInterface::GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags) const noexcept
{
	return m_rayTracingServices->GetTlasPrebuildInfo(instanceCount, buildFlags);
}

RhiOwnedResourceHandle VulkanRenderHardwareInterface::CreateRayTracingScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName)
{
	return m_rayTracingServices->CreateScratchBuffer(sizeInBytes, debugName);
}

RhiOwnedResourceHandle VulkanRenderHardwareInterface::CreateRayTracingAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName)
{
	return m_rayTracingServices->CreateAccelerationStructureBuffer(sizeInBytes, type, debugName);
}

RhiOwnedResourceHandle VulkanRenderHardwareInterface::CreateRayTracingInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName)
{
	return m_rayTracingServices->CreateInstanceBuffer(instances, instanceCount, debugName);
}

VkInstance VulkanRenderHardwareInterface::GetVulkanInstance() const noexcept
{
	return m_rhi->GetInstance();
}

VkPhysicalDevice VulkanRenderHardwareInterface::GetVulkanPhysicalDevice() const noexcept
{
	return m_rhi->GetPhysicalDevice();
}

VkDevice VulkanRenderHardwareInterface::GetVulkanDevice() const noexcept
{
	return m_rhi->GetDevice();
}

VkQueue VulkanRenderHardwareInterface::GetVulkanGraphicsQueue() const noexcept
{
	return m_rhi->GetGraphicsQueue();
}

std::uint32_t VulkanRenderHardwareInterface::GetVulkanGraphicsQueueFamilyIndex() const noexcept
{
	return m_rhi->GetGraphicsQueueFamilyIndex();
}

std::uint32_t VulkanRenderHardwareInterface::GetVulkanApiVersion() const noexcept
{
	return m_rhi->GetAdapterInfo().ApiVersion;
}

std::uint32_t VulkanRenderHardwareInterface::GetSwapChainBackBufferCount() const noexcept
{
	return m_swapChain->GetBackBufferCount();
}

VkFormat VulkanRenderHardwareInterface::GetNativeBackBufferFormat() const noexcept
{
	return m_swapChain->GetNativeBackBufferFormat();
}

void VulkanRenderHardwareInterface::SetCurrentFrameIndex(std::uint32_t frameIndex) noexcept
{
	m_currentFrameIndex = frameIndex;
}

void VulkanRenderHardwareInterface::SetCommandRecordingContext(VulkanCommandRecordingContext& commandRecordingContext) noexcept
{
	m_commandRecordingContext = &commandRecordingContext;
	m_presentationService->GetImplementation().SetCommandRecordingContext(commandRecordingContext);
	m_capabilities = BuildCapabilities();
}

void VulkanRenderHardwareInterface::ResetTransientFrameResources() noexcept
{
	m_descriptorService->BeginFrame(m_currentFrameIndex);
}

void VulkanRenderHardwareInterface::RebuildSwapChainBackBufferViews() noexcept
{
	m_presentationService->GetImplementation().RebuildSwapChainBackBufferViews();
}

void VulkanRenderHardwareInterface::PrepareCurrentBackBufferForPresentation(VulkanRenderCommandList& commandList) noexcept
{
	m_presentationService->GetImplementation().PrepareCurrentBackBufferForPresentation(commandList);
}

void VulkanRenderHardwareInterface::TransitionCurrentBackBuffer(VkCommandBuffer commandBuffer, ResourceState newState) noexcept
{
	m_presentationService->GetImplementation().TransitionCurrentBackBuffer(commandBuffer, newState);
}
