#include "PCH.h"

#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Device/D3D12ExternalFeatureInteropCapabilities.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"
#include "CVars/RHICVars.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "D3D12/Diagnostics/GpuCrash/D3D12GpuCrash.h"
#endif

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_d3d12RhiLogger, "RHI.D3D12");

static bool SupportsRequiredDevice(IDXGIAdapter1& adapter, D3D_FEATURE_LEVEL featureLevel) noexcept
{
	DXGI_ADAPTER_DESC1 desc{};
	if (FAILED(adapter.GetDesc1(&desc)) || (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
	{
		return false;
	}

	return SUCCEEDED(D3D12CreateDevice(&adapter, featureLevel, _uuidof(ID3D12Device), nullptr));
}

void D3D12Rhi::SelectAdapter() noexcept
{
	const DXGI_GPU_PREFERENCE pref = CVarPreferHighPerformanceAdapter.Get() ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

	for (UINT i = 0;; ++i)
	{
		ComPtr<IDXGIAdapter1> candidate;
		HRESULT hr = m_dxgiFactory->EnumAdapterByGpuPreference(i, pref, IID_PPV_ARGS(candidate.ReleaseAndGetAddressOf()));
		if (hr != S_OK)
		{
			break;
		}

		if (SupportsRequiredDevice(*candidate.Get(), m_desiredD3DFeatureLevel))
		{
			m_adapter = candidate;
			return;
		}
	}

	for (UINT i = 0;; ++i)
	{
		ComPtr<IDXGIAdapter1> candidate;
		HRESULT hr = m_dxgiFactory->EnumAdapters1(i, candidate.ReleaseAndGetAddressOf());
		if (hr != S_OK)
		{
			break;
		}

		if (SupportsRequiredDevice(*candidate.Get(), m_desiredD3DFeatureLevel))
		{
			m_adapter = candidate;
			return;
		}
	}
}

void D3D12Rhi::CheckShaderModel6Support() const noexcept
{
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
	shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_6;
	if (!m_device)
	{
		Diagnostics::Fatal(g_d3d12RhiLogger, __FILE__, __LINE__, "CheckShaderModel6Support called before device creation");
	}

	HRESULT hr = m_device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	if (FAILED(hr) || shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_6)
	{
		Diagnostics::Fatal(g_d3d12RhiLogger, __FILE__, __LINE__, "Device does not support the engine minimum Shader Model 6.6.");
	}
}

void D3D12Rhi::CreateFactory()
{
#if ENGINE_GPU_VALIDATION
	UINT dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
	UINT dxgiFactoryFlags = 0;
#endif
	ComPtr<IDXGIFactory7> createdFactory;
	CHECK(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(createdFactory.ReleaseAndGetAddressOf())));
	if (m_interposerHooks.ResolveNativeInterface != nullptr)
	{
		ComPtr<IDXGIFactory7> nativeFactory;
		if (TryResolveNativeInterface(ERhiInterposerInterfaceKind::PresentationFactory, createdFactory.Get(), IID_PPV_ARGS(nativeFactory.ReleaseAndGetAddressOf())))
		{
			m_dxgiFactory = std::move(nativeFactory);
			return;
		}
	}
	m_dxgiFactory = std::move(createdFactory);
}

void D3D12Rhi::CreateDevice()
{
	SelectAdapter();
	if (!m_adapter)
	{
		Diagnostics::Fatal(g_d3d12RhiLogger, __FILE__, __LINE__, "No suitable adapter found when creating device");
	}

	ComPtr<ID3D12Device10> createdDevice;
	CHECK(D3D12CreateDevice(m_adapter.Get(), m_desiredD3DFeatureLevel, IID_PPV_ARGS(createdDevice.ReleaseAndGetAddressOf())));
	if (m_interposerHooks.ResolveNativeInterface != nullptr)
	{
		ComPtr<ID3D12Device10> nativeDevice;
		if (TryResolveNativeInterface(ERhiInterposerInterfaceKind::GraphicsDevice, createdDevice.Get(), IID_PPV_ARGS(nativeDevice.ReleaseAndGetAddressOf())))
		{
			m_device = std::move(nativeDevice);
		}
	}
	if (m_device == nullptr)
	{
		m_device = std::move(createdDevice);
	}
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	D3D12GpuCrash::Initialize(m_device.Get());
#endif
	if (m_interposerHooks.DeviceCreated != nullptr)
	{
		m_interposerActive = m_interposerHooks.DeviceCreated(NativeGraphicsDeviceHandle{m_device.Get()}, BuildD3D12AdapterIdentity(this), m_interposerHooks.UserData);
	}
	CheckRayTracingSupport();
}

void D3D12Rhi::CreateMemoryAllocator()
{
	if (!m_adapter || !m_device)
	{
		Diagnostics::Fatal(g_d3d12RhiLogger, __FILE__, __LINE__, "CreateMemoryAllocator called before adapter and device creation");
	}

	m_memoryAllocator = std::make_unique<D3D12GpuMemoryAllocator>(m_adapter.Get(), m_device.Get());
}
