#pragma once

#include "Presentation/RhiPresentationService.h"

class RenderCommandList;
class D3D12Rhi;
class D3D12SwapChain;
class D3D12DescriptorService;
class D3D12ResourceService;
class D3D12CommandRecordingContext;
class D3D12RenderCommandList;

class D3D12BackBufferPresentation final
{
public:
	D3D12BackBufferPresentation(D3D12Rhi& rhi, D3D12SwapChain& swapChain, D3D12DescriptorService& descriptors, D3D12ResourceService& resources) noexcept;
	D3D12BackBufferPresentation(const D3D12BackBufferPresentation&) = delete;
	D3D12BackBufferPresentation& operator=(const D3D12BackBufferPresentation&) = delete;
	D3D12BackBufferPresentation(D3D12BackBufferPresentation&&) = delete;
	D3D12BackBufferPresentation& operator=(D3D12BackBufferPresentation&&) = delete;

	void SetCommandRecordingContext(D3D12CommandRecordingContext& context) noexcept;

	RhiViewport GetBackBufferViewport() const noexcept;
	RhiRect GetBackBufferScissorRect() const noexcept;
	RhiCpuDescriptorHandle GetBackBufferRenderTargetView() const noexcept;
	RhiResourceHandle GetBackBufferResource() const noexcept;
	void BeginPresentRenderPass(RhiClearColorView clearColor) noexcept;
	void BeginPresentOverlayPass() noexcept;
	void EndPresentRenderPass() noexcept;
	PixelFormat GetPresentColorFormat() const noexcept;

private:
	RhiCpuDescriptorHandle BindBackBuffer(RenderCommandList& commandList) noexcept;
	RenderCommandList& GetGraphicsCommandList() noexcept;

	D3D12Rhi* m_rhi;
	D3D12SwapChain* m_swapChain;
	D3D12DescriptorService* m_descriptorService;
	D3D12ResourceService* m_resourceService;
	D3D12CommandRecordingContext* m_commandRecordingContext = nullptr;
};
