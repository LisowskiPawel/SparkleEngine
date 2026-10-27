#include "PCH.h"

#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Commands/D3D12CommandQueue.h"
#include "D3D12/Diagnostics/D3D12DebugLayer.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"

D3D12Rhi::D3D12Rhi(RhiInterposerHooks interposerHooks) noexcept :
    m_interposerHooks(interposerHooks)
{
	InitializeDebugLayer();
	CreateFactory();
	CreateDevice();

	InitializeDeviceDiagnostics();

	CreateMemoryAllocator();

	CheckShaderModel6Support();
	CreateCommandQueues();
	constexpr uint32_t kInitialFrameIndex = 0;
	SetCurrentFrameIndex(kInitialFrameIndex);
}

void D3D12Rhi::SetCurrentFrameIndex(uint32_t frameInFlightIndex) noexcept
{
	m_currentFrameIndex = frameInFlightIndex;
}

uint32_t D3D12Rhi::GetCurrentFrameIndex() const noexcept
{
	return m_currentFrameIndex;
}

const ComPtr<IDXGIFactory7>& D3D12Rhi::GetDxgiFactory() const noexcept
{
	return m_dxgiFactory;
}

const ComPtr<IDXGIAdapter1>& D3D12Rhi::GetAdapter() const noexcept
{
	return m_adapter;
}

const ComPtr<ID3D12Device10>& D3D12Rhi::GetDevice() const noexcept
{
	return m_device;
}

D3D_FEATURE_LEVEL D3D12Rhi::GetDeviceFeatureLevel() const noexcept
{
	return m_desiredD3DFeatureLevel;
}

D3D12GpuMemoryAllocator& D3D12Rhi::GetMemoryAllocator() noexcept
{
	return *m_memoryAllocator;
}

const D3D12GpuMemoryAllocator& D3D12Rhi::GetMemoryAllocator() const noexcept
{
	return *m_memoryAllocator;
}

D3D12Rhi::~D3D12Rhi() noexcept
{
	ShutdownInterposer();

	for (std::unique_ptr<D3D12CommandQueue>& queue : m_queues)
	{
		queue.reset();
	}

	m_externalCommandQueue.Reset();
	m_externalDevice.Reset();
	m_memoryAllocator.reset();

#if ENGINE_GPU_VALIDATION || SPARKLE_WITH_GPU_CRASH_DEBUGGING
	m_debugLayer.reset();
#endif

	m_device.Reset();
	m_adapter.Reset();
	m_dxgiFactory.Reset();
}
