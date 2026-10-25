#include "D3D12/D3D12PCH.h"
#include "D3D12/Device/D3D12Capabilities.h"

#include "Device/RhiCapabilitySupport.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Device/D3D12ExternalFeatureInteropCapabilities.h"
#include "D3D12/SwapChain/D3D12SwapChain.h"
#include "D3D12/D3D12TypeConversions.h"
#include "Diagnostics/RhiDiagnostics.h"
#include "Shaders/ShaderTarget.h"

static RhiFormatSupport QueryFormatSupport(D3D12Rhi& rhi, PixelFormat format) noexcept
{
	RhiFormatSupport support{.Format = format};
	if (format == PixelFormat::Unknown)
	{
		return support;
	}

	D3D12_FEATURE_DATA_FORMAT_SUPPORT data{};
	data.Format = D3D12TypeConversions::ToDxgiFormat(format);
	const HRESULT supportResult = rhi.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &data, sizeof(data));
	if (data.Format == DXGI_FORMAT_UNKNOWN || FAILED(supportResult))
	{
		return support;
	}

	support.SupportsTexture = (data.Support1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D) != 0;
	support.SupportsShaderResource = (data.Support1 & (D3D12_FORMAT_SUPPORT1_SHADER_LOAD | D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE)) != 0;
	support.SupportsUnorderedAccess = (data.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE) != 0;
	support.SupportsRenderTarget = (data.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) != 0;
	support.SupportsDepthStencil = (data.Support1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL) != 0;
	return support;
}

RhiCapabilities BuildD3D12Capabilities(D3D12Rhi& rhi, D3D12SwapChain& swapChain, RenderDiagnostics& diagnostics) noexcept
{
	RhiCapabilities capabilities{};
	const D3D_FEATURE_LEVEL featureLevel = rhi.GetDeviceFeatureLevel();
	const std::uint32_t featureLevelMajor = featureLevel >= D3D_FEATURE_LEVEL_12_0 ? 12u : featureLevel >= D3D_FEATURE_LEVEL_11_0 ? 11u : 0u;

	const std::uint32_t featureLevelMinor = featureLevel == D3D_FEATURE_LEVEL_12_2 ? 2u
	    : featureLevel == D3D_FEATURE_LEVEL_12_1                                   ? 1u
	    : featureLevel == D3D_FEATURE_LEVEL_12_0                                   ? 0u
	    : featureLevel == D3D_FEATURE_LEVEL_11_1                                   ? 1u
	    : featureLevel == D3D_FEATURE_LEVEL_11_0                                   ? 0u
	                                                                               : 0u;

	capabilities.BackendApi = ERhiBackendApi::D3D12;
	capabilities.RuntimeShaderBinaryFormat = ShaderBinaryFormat::Dxil;

	capabilities.BackendVersion = RhiBackendVersionInfo{
	    .Semantic = ERhiBackendVersionSemantic::FeatureLevel,
	    .Major = featureLevelMajor,
	    .Minor = featureLevelMinor,
	    .Patch = 0,
	    .PackedValue = static_cast<std::uint32_t>(featureLevel)};

	capabilities.DescriptorModel = ERhiDescriptorModel::DescriptorTables;

	capabilities.BindingLimits = RhiBindingLimits{
	    .MaxDescriptorSets = 1,
	    .MaxShaderResourceDescriptors = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2,
	    .MaxSamplerDescriptors = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE,
	    .MaxDescriptorTableEntries = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2,
	    .MaxPushConstantBytes = 256};

	capabilities.DescriptorIndexing = RhiDescriptorIndexingCapabilities{.SupportsSampledImageArrayNonUniformIndexing = true, .SupportsPartiallyBoundDescriptorArrays = true};
	capabilities.UploadReadback = RhiUploadReadbackCapabilities{.SupportsBufferUpload = true, .SupportsTextureUpload = true, .SupportsReadback = true};

	capabilities.Presentation = RhiPresentationCapabilities{
	    .BackBufferCount = swapChain.GetBackBufferCount(),
	    .MaximumFramesInFlight = swapChain.GetMaximumFramesInFlight(),
	    .Throttle = ERhiPresentationThrottle::FrameLatencyWaitableObject};

	for (std::size_t index = 0; index < capabilities.FormatSupport.size(); ++index)
	{
		capabilities.FormatSupport[index] = QueryFormatSupport(rhi, kRhiCapabilityPixelFormats[index]);
	}
	capabilities.Diagnostics = BuildRhiBackendDiagnosticsSupport(rhi.IsValidationEnabled(), diagnostics);
	capabilities.RayTracing = rhi.GetRayTracingCapabilities();
	D3D12_FEATURE_DATA_D3D12_OPTIONS options{};
	capabilities.SupportsShaderFloat64 = SUCCEEDED(rhi.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options))) && options.DoublePrecisionFloatShaderOps != FALSE;
	capabilities.SupportsMeshShaders = false;
	capabilities.SupportsTaskShaders = false;
	capabilities.Queues.Set(ERhiQueueType::Graphics, true, true);
	capabilities.Queues.Set(ERhiQueueType::Compute, true, true);
	capabilities.Queues.Set(ERhiQueueType::Copy, true, true);
	capabilities.SupportsPresent = swapChain.GetBackBufferFormat() != PixelFormat::Unknown;
	capabilities.MemoryAllocator = ERhiMemoryAllocatorBackend::D3D12Managed;
	capabilities.MemorySupport = BuildRhiBackendMemorySupport(diagnostics);
	capabilities.ExternalFeatureInterop = BuildD3D12ExternalFeatureInteropCapabilities(&rhi, rhi.GetDevice() != nullptr);
	return capabilities;
}
