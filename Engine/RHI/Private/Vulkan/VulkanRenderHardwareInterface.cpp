#include "Vulkan/VulkanPCH.h"

#include "Vulkan/VulkanRenderHardwareInterface.h"

#include "Vulkan/Capture/VulkanCaptureService.h"
#include "Vulkan/Commands/VulkanCommandRecordingContext.h"
#include "Vulkan/Commands/VulkanRenderCommandList.h"
#include "Vulkan/Descriptors/VulkanDescriptorAllocator.h"
#include "Vulkan/Descriptors/VulkanDescriptorService.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Device/VulkanCapabilities.h"
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
	m_imguiBackend = std::make_unique<VulkanImGuiBackend>(rhi, swapChain, *m_descriptorService);
	m_diagnostics = CreateVulkanRenderDiagnostics(rhi, memoryAllocator);
	RebuildSwapChainBackBufferViews();
	m_capabilities = BuildVulkanCapabilities(*m_rhi, *m_swapChain, *m_diagnostics, m_commandRecordingContext != nullptr);
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

void VulkanRenderHardwareInterface::SetCurrentFrameIndex(std::uint32_t frameIndex) noexcept
{
	m_currentFrameIndex = frameIndex;
}

void VulkanRenderHardwareInterface::SetCommandRecordingContext(VulkanCommandRecordingContext& commandRecordingContext) noexcept
{
	m_commandRecordingContext = &commandRecordingContext;
	m_presentationService->GetImplementation().SetCommandRecordingContext(commandRecordingContext);
	m_capabilities = BuildVulkanCapabilities(*m_rhi, *m_swapChain, *m_diagnostics, m_commandRecordingContext != nullptr);
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
