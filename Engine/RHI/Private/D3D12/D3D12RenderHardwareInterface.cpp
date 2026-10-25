#include "PCH.h"

#include "D3D12/D3D12RenderHardwareInterface.h"

#include "Commands/RenderCommandList.h"
#include "D3D12/Commands/D3D12CommandRecordingContext.h"
#include "D3D12/Capture/D3D12CaptureService.h"
#include "D3D12/Device/D3D12Capabilities.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/SwapChain/D3D12SwapChain.h"
#include "D3D12/Diagnostics/D3D12RenderDiagnostics.h"
#include "D3D12/Descriptors/D3D12DescriptorHeap.h"
#include "D3D12/Descriptors/D3D12DescriptorHeapManager.h"
#include "D3D12/Descriptors/D3D12DescriptorService.h"
#include "D3D12/Interop/D3D12InteropService.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"
#include "D3D12/Pipeline/D3D12BindingLayout.h"
#include "D3D12/Pipeline/D3D12Pipeline.h"
#include "D3D12/Pipeline/D3D12RayTracingPipeline.h"
#include "D3D12/RayTracing/D3D12RayTracingServices.h"
#include "D3D12/Resources/D3D12ResourceService.h"
#include "D3D12/Resources/D3D12UploadService.h"
#include "D3D12/Samplers/D3D12SamplerLibrary.h"
#include "D3D12/UI/D3D12ImGuiBackend.h"
#include "Presentation/RhiPresentationServiceAdapter.h"
#include "D3D12/SwapChain/D3D12BackBufferPresentation.h"
#include "Pipeline/RhiPipelineServiceAdapter.h"

#include <d3d12.h>

D3D12RenderHardwareInterface::D3D12RenderHardwareInterface(
    D3D12Rhi& rhi,
    D3D12GpuMemoryAllocator& memoryAllocator,
    D3D12DescriptorHeapManager& descriptorHeapManager,
    D3D12SwapChain& swapChain,
    D3D12UploadService& uploadService) noexcept :
    m_rhi(&rhi),
    m_descriptorHeapManager(&descriptorHeapManager),
    m_swapChain(&swapChain),
    m_uploadService(&uploadService)
{
	m_interopService = std::make_unique<D3D12InteropService>(*this);
	m_captureService = std::make_unique<D3D12CaptureService>(rhi);
	m_pipelineService = std::make_unique<RhiPipelineServiceAdapter<D3D12Rhi, D3D12Pipeline, D3D12RayTracingPipeline, D3D12BindingLayoutCompiler>>(rhi);
	m_descriptorService = std::make_unique<D3D12DescriptorService>(rhi, descriptorHeapManager, m_capabilities);
	m_resourceService = std::make_unique<D3D12ResourceService>(rhi, memoryAllocator, m_capabilities);
	m_presentationService = std::make_unique<RhiPresentationServiceAdapter<D3D12BackBufferPresentation>>(rhi, swapChain, *m_descriptorService, *m_resourceService);

	m_rayTracingServices = std::make_unique<D3D12RayTracingServices>(rhi, memoryAllocator, *m_resourceService, rhi.GetNvapiRayTracingProvider());

	m_diagnostics = CreateD3D12RenderDiagnostics(rhi, swapChain.GetMaximumFramesInFlight());
	m_capabilities = BuildD3D12Capabilities(*m_rhi, *m_swapChain, *m_diagnostics);
	m_imguiBackend = std::make_unique<D3D12ImGuiBackend>(*this);
}

D3D12RenderHardwareInterface::~D3D12RenderHardwareInterface() noexcept = default;

D3D12RecordingResourceUseToken D3D12RenderHardwareInterface::BeginResourceTracking(RhiResourceHandle resource, bool coordinatorRecording) noexcept
{
	return m_resourceService->BeginResourceTracking(resource, coordinatorRecording);
}

void D3D12RenderHardwareInterface::EndResourceTracking(D3D12RecordingResourceUseToken use, RhiSubmissionToken submissionToken) noexcept
{
	m_resourceService->EndResourceTracking(use, submissionToken);
}

std::uint32_t D3D12RenderHardwareInterface::GetCurrentFrameIndex() const noexcept
{
	return m_rhi->GetCurrentFrameIndex();
}

RhiResourceService& D3D12RenderHardwareInterface::GetResourceService() noexcept
{
	return *m_resourceService;
}

const RhiResourceService& D3D12RenderHardwareInterface::GetResourceService() const noexcept
{
	return *m_resourceService;
}

RhiDescriptorService& D3D12RenderHardwareInterface::GetDescriptorService() noexcept
{
	return *m_descriptorService;
}

const RhiDescriptorService& D3D12RenderHardwareInterface::GetDescriptorService() const noexcept
{
	return *m_descriptorService;
}

RhiPipelineService& D3D12RenderHardwareInterface::GetPipelineService() noexcept
{
	return *m_pipelineService;
}

RhiUploadService& D3D12RenderHardwareInterface::GetUploadService() noexcept
{
	return *m_uploadService;
}

const RhiUploadService& D3D12RenderHardwareInterface::GetUploadService() const noexcept
{
	return *m_uploadService;
}

RhiRayTracingService& D3D12RenderHardwareInterface::GetRayTracingService() noexcept
{
	return *m_rayTracingServices;
}

const RhiRayTracingService& D3D12RenderHardwareInterface::GetRayTracingService() const noexcept
{
	return *m_rayTracingServices;
}

void D3D12RenderHardwareInterface::WaitForIdle() noexcept
{
	m_rhi->WaitForIdle();
	m_resourceService->FlushDeferredResourceReleases();
}

RhiInteropService& D3D12RenderHardwareInterface::GetInteropService() noexcept
{
	return *m_interopService;
}

const RhiInteropService& D3D12RenderHardwareInterface::GetInteropService() const noexcept
{
	return *m_interopService;
}

RhiCaptureService& D3D12RenderHardwareInterface::GetCaptureService() noexcept
{
	return *m_captureService;
}

RhiPresentationService& D3D12RenderHardwareInterface::GetPresentationService() noexcept
{
	return *m_presentationService;
}

const RhiPresentationService& D3D12RenderHardwareInterface::GetPresentationService() const noexcept
{
	return *m_presentationService;
}

NativeGraphicsDeviceHandle D3D12RenderHardwareInterface::GetDeviceHandle() const noexcept
{
	return NativeGraphicsDeviceHandle{m_rhi->GetDevice().Get()};
}

NativeGraphicsQueueHandle D3D12RenderHardwareInterface::GetGraphicsQueueHandle() const noexcept
{
	return NativeGraphicsQueueHandle{m_rhi->GetCommandQueue().Get()};
}

RenderCommandList& D3D12RenderHardwareInterface::GetGraphicsCommandList(std::uint32_t frameIndex) noexcept
{
	return GetCommandList(ERhiQueueType::Graphics, frameIndex);
}

RenderCommandList& D3D12RenderHardwareInterface::GetCommandList(ERhiQueueType queueType, std::uint32_t frameIndex) noexcept
{
	m_resourceService->DrainCompletedResourceReleases();
	return m_commandRecordingContext->GetCurrentCommandList(queueType, frameIndex);
}

void D3D12RenderHardwareInterface::SetCommandRecordingContext(D3D12CommandRecordingContext& commandContext) noexcept
{
	m_commandRecordingContext = &commandContext;
	m_presentationService->GetImplementation().SetCommandRecordingContext(commandContext);
}

RhiRayTracingCapabilities D3D12RenderHardwareInterface::GetRayTracingCapabilities() const noexcept
{
	return m_rayTracingServices->GetCapabilities();
}

RenderDiagnostics& D3D12RenderHardwareInterface::GetDiagnostics() noexcept
{
	return *m_diagnostics;
}

const RenderDiagnostics& D3D12RenderHardwareInterface::GetDiagnostics() const noexcept
{
	return *m_diagnostics;
}

RhiImGuiRenderer& D3D12RenderHardwareInterface::GetImGuiRenderer() noexcept
{
	return *m_imguiBackend;
}

ID3D12DescriptorHeap* D3D12RenderHardwareInterface::GetD3D12ShaderResourceDescriptorHeap() const noexcept
{
	return m_descriptorService->GetShaderResourceDescriptorHeap();
}

RhiRayTracingAccelerationStructurePrebuildInfo D3D12RenderHardwareInterface::GetBottomLevelAccelerationStructurePrebuildInfo(const RhiRayTracingGeometryDesc& geometry) const noexcept
{
	return m_rayTracingServices->GetBottomLevelAccelerationStructurePrebuildInfo(geometry);
}

RhiRayTracingAccelerationStructurePrebuildInfo D3D12RenderHardwareInterface::GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags) const noexcept
{
	return m_rayTracingServices->GetTlasPrebuildInfo(instanceCount, buildFlags);
}

RhiOwnedResourceHandle D3D12RenderHardwareInterface::CreateRayTracingScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName)
{
	return m_rayTracingServices->CreateScratchBuffer(sizeInBytes, debugName);
}

RhiOwnedResourceHandle D3D12RenderHardwareInterface::CreateRayTracingAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName)
{
	return m_rayTracingServices->CreateAccelerationStructureBuffer(sizeInBytes, type, debugName);
}

RhiOwnedResourceHandle D3D12RenderHardwareInterface::CreateRayTracingInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName)
{
	return m_rayTracingServices->CreateInstanceBuffer(instances, instanceCount, debugName);
}

void D3D12RenderHardwareInterface::SetSamplerTableHandle(RhiDescriptorTableHandle samplerTableHandle) noexcept
{
	m_descriptorService->SetSamplerTableHandle(samplerTableHandle);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderHardwareInterface::ResolveDescriptorTableCpuHandle(RhiDescriptorTableHandle tableHandle, std::uint32_t descriptorIndex) const noexcept
{
	const RhiCpuDescriptorHandle handle = m_descriptorService->GetDescriptorTableCpuHandle(tableHandle, descriptorIndex);
	return D3D12_CPU_DESCRIPTOR_HANDLE{handle.Value};
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12RenderHardwareInterface::ResolveDescriptorTableGpuHandle(RhiDescriptorTableHandle tableHandle, std::uint32_t descriptorIndex) const noexcept
{
	const RhiGpuDescriptorHandle handle = m_descriptorService->GetDescriptorTableGpuHandle(tableHandle, descriptorIndex);
	return D3D12_GPU_DESCRIPTOR_HANDLE{handle.Value};
}

bool D3D12RenderHardwareInterface::BuildPtlas(ID3D12GraphicsCommandList7* commandList, const RhiPtlasBuildCommandDesc& desc) const noexcept
{
	return m_rayTracingServices->BuildPtlas(commandList, desc);
}
