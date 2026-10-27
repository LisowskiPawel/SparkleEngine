#include "PCH.h"

#include "D3D12/Device/D3D12Rhi.h"

ComPtr<ID3D12CommandQueue> D3D12Rhi::CreateInterposerGraphicsQueue() noexcept
{
	ComPtr<ID3D12CommandQueue> nativeGraphicsQueue;
	D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
	graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	graphicsQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	graphicsQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	graphicsQueueDesc.NodeMask = 0;
	if (m_interposerActive)
	{
		ComPtr<ID3D12Device10> externalDevice;
		if (TryUpgradeInterposerInterface(ERhiInterposerInterfaceKind::GraphicsDevice, m_device.Get(), IID_PPV_ARGS(externalDevice.ReleaseAndGetAddressOf())))
		{
			ComPtr<ID3D12CommandQueue> externalQueue;
			const HRESULT createResult = externalDevice->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(externalQueue.ReleaseAndGetAddressOf()));
			if (SUCCEEDED(createResult))
			{
				ComPtr<ID3D12CommandQueue> nativeQueue;
				if (TryResolveNativeInterface(ERhiInterposerInterfaceKind::GraphicsQueue, externalQueue.Get(), IID_PPV_ARGS(nativeQueue.ReleaseAndGetAddressOf())))
				{
					m_externalDevice = std::move(externalDevice);
					m_externalCommandQueue = std::move(externalQueue);
					nativeGraphicsQueue = std::move(nativeQueue);
				}
			}
		}

		if (nativeGraphicsQueue == nullptr)
		{
			DisableInterposer();
		}
	}

	return nativeGraphicsQueue;
}

bool D3D12Rhi::TryUpgradeInterposerInterface(ERhiInterposerInterfaceKind kind, IUnknown* nativeInterface, REFIID requestedInterface, void** upgradedInterface) noexcept
{
	if (!m_interposerActive || m_interposerHooks.UpgradeInterface == nullptr || nativeInterface == nullptr || upgradedInterface == nullptr)
	{
		return false;
	}

	*upgradedInterface = nullptr;
	void* candidate = nativeInterface;
	if (!m_interposerHooks.UpgradeInterface(kind, &candidate, m_interposerHooks.UserData) || candidate == nullptr)
	{
		return false;
	}

	IUnknown* candidateInterface = static_cast<IUnknown*>(candidate);
	const HRESULT result = candidateInterface->QueryInterface(requestedInterface, upgradedInterface);
	if (candidateInterface != nativeInterface)
	{
		// Interface upgrades transfer the initial proxy reference. QueryInterface
		// above produced the reference retained by the RHI.
		candidateInterface->Release();
	}
	return SUCCEEDED(result) && *upgradedInterface != nullptr;
}

bool D3D12Rhi::TryResolveNativeInterface(ERhiInterposerInterfaceKind kind, IUnknown* externalInterface, REFIID requestedInterface, void** nativeInterface) noexcept
{
	if (m_interposerHooks.ResolveNativeInterface == nullptr || externalInterface == nullptr || nativeInterface == nullptr)
	{
		return false;
	}

	*nativeInterface = nullptr;
	void* resolved = nullptr;
	if (!m_interposerHooks.ResolveNativeInterface(kind, externalInterface, &resolved, m_interposerHooks.UserData) || resolved == nullptr)
	{
		return false;
	}

	IUnknown* resolvedInterface = static_cast<IUnknown*>(resolved);
	const HRESULT result = resolvedInterface->QueryInterface(requestedInterface, nativeInterface);
	// Native-interface resolution transfers one reference regardless of whether
	// the returned pointer aliases the input.
	resolvedInterface->Release();
	return SUCCEEDED(result) && *nativeInterface != nullptr;
}

void D3D12Rhi::NotifyInterposerPresentationReady(bool ready) noexcept
{
	if (m_interposerHooks.PresentationReady != nullptr)
	{
		m_interposerHooks.PresentationReady(ready && m_interposerActive, m_interposerHooks.UserData);
	}
}

void D3D12Rhi::NotifyFrameLatencyMarker(ERhiFrameLatencyMarker marker, std::uint64_t frameId) noexcept
{
	if (m_interposerActive && m_interposerHooks.FrameMarker != nullptr)
	{
		m_interposerHooks.FrameMarker(marker, frameId, m_interposerHooks.UserData);
	}
}

void D3D12Rhi::DisableInterposer() noexcept
{
	m_interposerActive = false;
	NotifyInterposerPresentationReady(false);
}

void D3D12Rhi::ShutdownInterposer() noexcept
{
	DisableInterposer();
	const RhiRuntimeShutdownCallback shutdown = m_interposerHooks.RuntimeShutdown;
	void* const userData = m_interposerHooks.UserData;
	m_interposerHooks = {};
	if (shutdown != nullptr)
	{
		shutdown(userData);
	}
}
