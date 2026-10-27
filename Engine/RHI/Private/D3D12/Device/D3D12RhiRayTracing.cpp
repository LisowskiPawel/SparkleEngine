#include "PCH.h"

#include "D3D12/Device/D3D12Rhi.h"
#include "CVars/RHICVars.h"

static constexpr std::uint32_t kD3D12RayTracingMaxDeclarableShaderPayloadSizeInBytes = 4096;
static constexpr std::uint32_t kNvidiaVendorId = 0x10DE;

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_d3d12RhiLogger, "RHI.D3D12");

bool D3D12Rhi::IsNvidiaAdapter() const noexcept
{
	if (m_adapter == nullptr)
	{
		return false;
	}

	DXGI_ADAPTER_DESC1 adapterDesc{};
	return SUCCEEDED(m_adapter->GetDesc1(&adapterDesc)) && adapterDesc.VendorId == kNvidiaVendorId;
}

void D3D12Rhi::CheckRayTracingSupport() noexcept
{
	m_rayTracingCapabilities = {};

	m_rayTracingCapabilities.Groups.Ptlas = RhiPtlasCapabilities{
	    .Supported = false,
	    .Provider = ERhiPtlasProvider::D3D12NvapiPtlas,
	    .NvidiaDeviceOnly = true,
	    .CurrentDeviceIsNvidia = IsNvidiaAdapter(),
	    .CapabilityStatusReason = "d3d12-options5-not-queried"};

	m_rayTracingCapabilities.Groups.Provider = RhiRayTracingProviderCapabilities{.SelectedTlasProvider = ERhiRayTracingTlasProvider::None, .SelectedTlasProviderReason = "ray-tracing-not-queried"};

	if (!m_device)
	{
		Diagnostics::Fatal(g_d3d12RhiLogger, __FILE__, __LINE__, "CheckRayTracingSupport called before device creation");
		return;
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5{};
	const HRESULT hr = m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));
	if (SUCCEEDED(hr))
	{
		m_rayTracingCapabilities.SupportsAccelerationStructure = options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0;
		m_rayTracingCapabilities.SupportsInlineRayQuery = options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_1;
		m_rayTracingCapabilities.SupportsRayTracingPipeline = options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0;
		if (m_rayTracingCapabilities.SupportsAccelerationStructure)
		{
			m_rayTracingCapabilities.MaxTraceRecursionDepth = D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH;
			m_rayTracingCapabilities.MaxRayPayloadSizeInBytes = std::min(kD3D12RayTracingMaxDeclarableShaderPayloadSizeInBytes, kRhiRayTracingMaxPayloadSizeInBytes);
			m_rayTracingCapabilities.MaxRayAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
			m_rayTracingCapabilities.ShaderGroupHandleSizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
			m_rayTracingCapabilities.ShaderTableAlignmentInBytes = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
			m_rayTracingCapabilities.ShaderTableRecordAlignmentInBytes = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
			m_rayTracingCapabilities.MaxShaderTableRecordStrideInBytes = D3D12_RAYTRACING_MAX_SHADER_RECORD_STRIDE;
			m_rayTracingCapabilities.AccelerationStructureByteAlignment = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT;
			m_rayTracingCapabilities.ScratchBufferByteAlignment = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT;
			m_rayTracingCapabilities.InstanceDescSizeInBytes = sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
		}

		PopulateStandardRayTracingCapabilityGroups(m_rayTracingCapabilities);

		m_rayTracingCapabilities.Groups.Ptlas = RhiPtlasCapabilities{
		    .Supported = false,
		    .Provider = ERhiPtlasProvider::D3D12NvapiPtlas,
		    .NvidiaDeviceOnly = true,
		    .CurrentDeviceIsNvidia = IsNvidiaAdapter(),
		    .CapabilityStatusReason = "d3d12-nvapi-ptlas-provider-not-queried"};

		m_rayTracingCapabilities.Groups.Ptlas = m_nvapiRayTracingProvider.QueryPtlasCapabilities(m_device.Get(), IsNvidiaAdapter(), m_rayTracingCapabilities.SupportsAccelerationStructure);

		SelectRayTracingTlasProvider();
	}
	else
	{
		m_rayTracingCapabilities.Groups.Ptlas.CapabilityStatusReason = "d3d12-options5-query-failed";
		m_rayTracingCapabilities.Groups.Provider.SelectedTlasProvider = ERhiRayTracingTlasProvider::None;
		m_rayTracingCapabilities.Groups.Provider.SelectedTlasProviderReason = "d3d12-options5-query-failed";
		SPDLOG_LOGGER_WARN(g_d3d12RhiLogger, "CheckFeatureSupport(OPTIONS5) failed hr={:#010x}; ray tracing assumed unsupported.", static_cast<uint32_t>(hr));
	}
}

void D3D12Rhi::SelectRayTracingTlasProvider() noexcept
{
	RhiRayTracingProviderCapabilities& provider = m_rayTracingCapabilities.Groups.Provider;
	if (!m_rayTracingCapabilities.SupportsAccelerationStructure)
	{
		provider = RhiRayTracingProviderCapabilities{.SelectedTlasProvider = ERhiRayTracingTlasProvider::None, .SelectedTlasProviderReason = "ray-tracing-unavailable"};
		return;
	}

	const RhiPtlasCapabilities& ptlas = m_rayTracingCapabilities.Groups.Ptlas;
	const bool ptlasSelected = CVarRayTracingPreferPtlas.Get() && ptlas.Supported && ptlas.SupportsDescriptorAccess;

	provider = RhiRayTracingProviderCapabilities{
	    .SelectedTlasProvider = ptlasSelected ? ERhiRayTracingTlasProvider::Ptlas : ERhiRayTracingTlasProvider::Tlas,
	    .SelectedTlasProviderReason = ptlasSelected ? "d3d12-nvapi-ptlas-selected" : "TLAS-selected"};
}

RhiRayTracingCapabilities D3D12Rhi::GetRayTracingCapabilities() const noexcept
{
	return m_rayTracingCapabilities;
}

D3D12NvapiRayTracingProvider& D3D12Rhi::GetNvapiRayTracingProvider() noexcept
{
	return m_nvapiRayTracingProvider;
}

const D3D12NvapiRayTracingProvider& D3D12Rhi::GetNvapiRayTracingProvider() const noexcept
{
	return m_nvapiRayTracingProvider;
}
