#pragma once

#include "Interop/RhiNativeHandles.h"
#include "UI/RhiImGuiRenderer.h"

#include <cstdint>
#include <d3d12.h>

struct ImDrawData;
struct ImGuiContext;
struct ImGui_ImplDX12_InitInfo;

class D3D12Rhi;
class D3D12SwapChain;
class D3D12DescriptorService;

class D3D12ImGuiBackend final : public RhiImGuiRenderer
{
public:
	D3D12ImGuiBackend(D3D12Rhi& rhi, D3D12SwapChain& swapChain, D3D12DescriptorService& descriptors) noexcept;

	void Initialize() override;
	void BeginFrame() noexcept override;
	std::uint64_t ResolveTextureId(RhiGpuDescriptorHandle shaderResourceView) noexcept override;
	void UpdateTexture(ImTextureData& texture) noexcept override;
	void RenderDrawData(RenderCommandList& commandList, ImDrawData* drawData) noexcept override;
	void Shutdown() noexcept override;

private:
	void InitializeRendererBackend(ImGuiContext* previousContext);
	void RenderNativeDrawData(NativeGraphicsCommandListHandle commandList, ImDrawData* drawData) noexcept;
	static void AllocateDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle);
	static void ReleaseDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	static ID3D12GraphicsCommandList* ToD3D12GraphicsCommandList(NativeGraphicsCommandListHandle handle) noexcept;
	static D3D12_CPU_DESCRIPTOR_HANDLE ToD3D12CpuDescriptor(RhiCpuDescriptorHandle handle) noexcept;
	static D3D12_GPU_DESCRIPTOR_HANDLE ToD3D12GpuDescriptor(RhiGpuDescriptorHandle handle) noexcept;

	D3D12Rhi& m_rhi;
	D3D12SwapChain& m_swapChain;
	D3D12DescriptorService& m_descriptorService;
	ImGuiContext* m_imguiContext = nullptr;
	bool m_ownsContext = false;
};
