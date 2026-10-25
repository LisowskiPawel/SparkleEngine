#include "D3D12/D3D12PCH.h"
#include "D3D12/SwapChain/D3D12BackBufferPresentation.h"

#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/SwapChain/D3D12SwapChain.h"
#include "D3D12/Descriptors/D3D12DescriptorService.h"
#include "D3D12/Resources/D3D12ResourceService.h"
#include "D3D12/Commands/D3D12CommandRecordingContext.h"
#include "D3D12/Commands/D3D12RenderCommandList.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_backBufferPresentationLogger, "RHI.D3D12.Presentation");

D3D12BackBufferPresentation::D3D12BackBufferPresentation(D3D12Rhi& rhi, D3D12SwapChain& swapChain, D3D12DescriptorService& descriptors, D3D12ResourceService& resources) noexcept :
    m_rhi(&rhi),
    m_swapChain(&swapChain),
    m_descriptorService(&descriptors),
    m_resourceService(&resources)
{
}

void D3D12BackBufferPresentation::SetCommandRecordingContext(D3D12CommandRecordingContext& context) noexcept
{
	m_commandRecordingContext = &context;
}

RenderCommandList& D3D12BackBufferPresentation::GetGraphicsCommandList() noexcept
{
	m_resourceService->DrainCompletedResourceReleases();
	return m_commandRecordingContext->GetCurrentCommandList(ERhiQueueType::Graphics, m_rhi->GetCurrentFrameIndex());
}

RhiViewport D3D12BackBufferPresentation::GetBackBufferViewport() const noexcept
{
	return m_swapChain->GetDefaultViewport();
}

RhiRect D3D12BackBufferPresentation::GetBackBufferScissorRect() const noexcept
{
	return m_swapChain->GetDefaultScissorRect();
}

RhiCpuDescriptorHandle D3D12BackBufferPresentation::GetBackBufferRenderTargetView() const noexcept
{
	return RhiCpuDescriptorHandle{m_swapChain->GetCPUHandle().ptr};
}

RhiResourceHandle D3D12BackBufferPresentation::GetBackBufferResource() const noexcept
{
	return RhiResourceHandle{m_swapChain->GetCurrentResource()};
}

RenderCommandList& D3D12BackBufferPresentation::BeginBackBufferRendering() noexcept
{
	RhiResourceHandle presentTexture{m_swapChain->GetCurrentResource()};
	RenderCommandList& commandList = GetGraphicsCommandList();
	commandList.TransitionResource(presentTexture, ResourceState::Present, ResourceState::RenderTarget);
	m_descriptorService->BindGlobalDescriptorState(commandList);

	const RhiCpuDescriptorHandle renderTargetView = GetBackBufferRenderTargetView();
	commandList.SetRenderTarget(renderTargetView);

	return commandList;
}

void D3D12BackBufferPresentation::BeginPresentRenderPass(RhiClearColorView clearColor) noexcept
{
	RenderCommandList& commandList = BeginBackBufferRendering();
	commandList.ClearRenderTarget(GetBackBufferRenderTargetView(), clearColor);
}

void D3D12BackBufferPresentation::BeginPresentOverlayPass() noexcept
{
	(void) BeginBackBufferRendering();
}

void D3D12BackBufferPresentation::EndPresentRenderPass() noexcept
{
	RhiResourceHandle presentTexture{m_swapChain->GetCurrentResource()};
	RenderCommandList& commandList = GetGraphicsCommandList();
	commandList.TransitionResource(presentTexture, ResourceState::RenderTarget, ResourceState::Present);
}

PixelFormat D3D12BackBufferPresentation::GetPresentColorFormat() const noexcept
{
	return m_swapChain->GetBackBufferFormat();
}
