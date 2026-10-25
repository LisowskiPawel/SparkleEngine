#include "PCH.h"

#include "D3D12/UI/D3D12ImGuiBackend.h"

#include "Commands/RenderCommandList.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/SwapChain/D3D12SwapChain.h"
#include "D3D12/Descriptors/D3D12DescriptorService.h"
#include "UI/RhiImGuiContext.h"
#include "D3D12/D3D12TypeConversions.h"

#include <backends/imgui_impl_dx12.h>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_d3d12ImGuiBackendLogger, "RHI.D3D12.ImGui");

D3D12ImGuiBackend::D3D12ImGuiBackend(D3D12Rhi& rhi, D3D12SwapChain& swapChain, D3D12DescriptorService& descriptors) noexcept :
    m_rhi(rhi),
    m_swapChain(swapChain),
    m_descriptorService(descriptors)
{
}

void D3D12ImGuiBackend::Initialize()
{
	if (m_imguiContext != nullptr)
	{
		return;
	}

	ImGuiContext* previousContext = AcquireImGuiContext(m_imguiContext, m_ownsContext);

	InitializeRendererBackend(previousContext);
	RestoreImGuiContext(previousContext);
}

void D3D12ImGuiBackend::InitializeRendererBackend(ImGuiContext* previousContext)
{
	ImGui_ImplDX12_InitInfo initInfo = {};
	initInfo.Device = m_rhi.GetDevice().Get();
	initInfo.CommandQueue = m_rhi.GetCommandQueue().Get();
	initInfo.NumFramesInFlight = static_cast<int>(m_swapChain.GetMaximumFramesInFlight());
	initInfo.RTVFormat = D3D12TypeConversions::ToDxgiFormat(m_swapChain.GetBackBufferFormat());
	initInfo.DSVFormat = D3D12TypeConversions::ToDxgiFormat(PixelFormat::Unknown);
	initInfo.SrvDescriptorHeap = m_descriptorService.GetShaderResourceDescriptorHeap();
	initInfo.SrvDescriptorAllocFn = &D3D12ImGuiBackend::AllocateDescriptor;
	initInfo.SrvDescriptorFreeFn = &D3D12ImGuiBackend::ReleaseDescriptor;
	initInfo.UserData = &m_descriptorService;

	if (initInfo.Device == nullptr || initInfo.CommandQueue == nullptr || initInfo.SrvDescriptorHeap == nullptr)
	{
		RestoreImGuiContext(previousContext);
		Diagnostics::Fatal(g_d3d12ImGuiBackendLogger, __FILE__, __LINE__, "Cannot initialize ImGui with incomplete D3D12 device state.");
	}

	if (!ImGui_ImplDX12_Init(&initInfo))
	{
		RestoreImGuiContext(previousContext);
		ReleaseImGuiContext(m_imguiContext, m_ownsContext);
		Diagnostics::Fatal(g_d3d12ImGuiBackendLogger, __FILE__, __LINE__, "ImGui D3D12 renderer initialization failed.");
	}
}

void D3D12ImGuiBackend::BeginFrame() noexcept
{
	ImGuiContext* previousContext = ActivateImGuiContext(m_imguiContext);
	ImGui_ImplDX12_NewFrame();
	RestoreImGuiContext(previousContext);
}

std::uint64_t D3D12ImGuiBackend::ResolveTextureId(RhiGpuDescriptorHandle shaderResourceView) noexcept
{
	return shaderResourceView.Value;
}

void D3D12ImGuiBackend::UpdateTexture(ImTextureData& texture) noexcept
{
	ImGuiContext* previousContext = ActivateImGuiContext(m_imguiContext);
	ImGui_ImplDX12_UpdateTexture(&texture);
	RestoreImGuiContext(previousContext);
}

void D3D12ImGuiBackend::RenderDrawData(RenderCommandList& commandList, ImDrawData* drawData) noexcept
{
	ImGuiContext* previousContext = ActivateImGuiContext(m_imguiContext);
	RenderNativeDrawData(commandList.GetNativeHandle(RhiNativeInteropRequest{.Consumer = ERhiNativeInteropConsumer::Presentation, .Reason = "Render ImGui draw data through D3D12 backend"}), drawData);
	RestoreImGuiContext(previousContext);
}

void D3D12ImGuiBackend::RenderNativeDrawData(NativeGraphicsCommandListHandle commandList, ImDrawData* drawData) noexcept
{
	ID3D12GraphicsCommandList* nativeCommandList = ToD3D12GraphicsCommandList(commandList);
	if (nativeCommandList == nullptr || drawData == nullptr)
	{
		return;
	}

	ImGui_ImplDX12_RenderDrawData(drawData, nativeCommandList);
}

void D3D12ImGuiBackend::Shutdown() noexcept
{
	if (m_imguiContext == nullptr)
	{
		return;
	}
	ImGuiContext* previousContext = ActivateImGuiContext(m_imguiContext);
	ImGui_ImplDX12_InvalidateDeviceObjects();
	ImGui_ImplDX12_Shutdown();
	RestoreImGuiContext(previousContext);
	ReleaseImGuiContext(m_imguiContext, m_ownsContext);
}

void D3D12ImGuiBackend::AllocateDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle)
{
	auto* descriptors = static_cast<D3D12DescriptorService*>(info->UserData);
	const RhiDescriptorAllocation allocation = descriptors->AllocateDescriptor(ERhiDescriptorAllocatorType::ShaderResource);
	*outCpuHandle = ToD3D12CpuDescriptor(allocation.CpuHandle);
	*outGpuHandle = ToD3D12GpuDescriptor(allocation.GpuHandle);
}

void D3D12ImGuiBackend::ReleaseDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	auto* descriptors = static_cast<D3D12DescriptorService*>(info->UserData);

	descriptors->ReleaseDescriptor(
	    ERhiDescriptorAllocatorType::ShaderResource,
	    RhiDescriptorAllocation{.CpuHandle = RhiCpuDescriptorHandle{cpuHandle.ptr}, .GpuHandle = RhiGpuDescriptorHandle{gpuHandle.ptr}});
}

ID3D12GraphicsCommandList* D3D12ImGuiBackend::ToD3D12GraphicsCommandList(NativeGraphicsCommandListHandle handle) noexcept
{
	return static_cast<ID3D12GraphicsCommandList*>(handle.Value);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12ImGuiBackend::ToD3D12CpuDescriptor(RhiCpuDescriptorHandle handle) noexcept
{
	D3D12_CPU_DESCRIPTOR_HANDLE nativeHandle{};
	nativeHandle.ptr = handle.Value;
	return nativeHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12ImGuiBackend::ToD3D12GpuDescriptor(RhiGpuDescriptorHandle handle) noexcept
{
	D3D12_GPU_DESCRIPTOR_HANDLE nativeHandle{};
	nativeHandle.ptr = handle.Value;
	return nativeHandle;
}
