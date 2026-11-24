#include "PCH.h"
#include "D3D12/Samplers/D3D12SamplerLibrary.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/D3D12TypeConversions.h"

#include <algorithm>
#include <array>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_samplerLibraryLogger, "RHI.D3D12.Samplers");

static const std::array<RhiSamplerDesc, 31>& GetSharedSamplerDescs() noexcept
{
	static const auto samplers = []
	{
		std::array<RhiSamplerDesc, 31> descriptions{};
		std::size_t index = 0;
		for (const auto filter : {RhiSamplerMinMagFilter::Point, RhiSamplerMinMagFilter::Linear})
		{
			for (const auto mip : {RhiSamplerMipFilter::Point, RhiSamplerMipFilter::Linear, RhiSamplerMipFilter::None})
			{
				for (const auto address : {RhiSamplerAddressMode::Wrap, RhiSamplerAddressMode::Clamp, RhiSamplerAddressMode::Mirror})
				{
					descriptions[index++] = {.MinMagFilter = filter, .MipFilter = mip, .Address = MakeRhiSamplerAddressModes(address)};
				}
			}
		}

		descriptions[index++] = {
		    .MinMagFilter = RhiSamplerMinMagFilter::Linear,
		    .MipFilter = RhiSamplerMipFilter::None,
		    .Address = {.U = RhiSamplerAddressMode::Wrap, .V = RhiSamplerAddressMode::Clamp, .W = RhiSamplerAddressMode::Clamp}};

		for (const auto anisotropy : {RhiSamplerAnisotropy::X2, RhiSamplerAnisotropy::X4, RhiSamplerAnisotropy::X8, RhiSamplerAnisotropy::X16})
		{
			for (const auto address : {RhiSamplerAddressMode::Wrap, RhiSamplerAddressMode::Clamp, RhiSamplerAddressMode::Mirror})
			{
				descriptions[index++] = {.Address = MakeRhiSamplerAddressModes(address), .MaxAnisotropy = anisotropy};
			}
		}
		return descriptions;
	}();

	return samplers;
}

bool D3D12SamplerLibrary::TryGetSlot(const RhiSamplerDesc& samplerDesc, std::uint32_t& outSlot) noexcept
{
	RhiSamplerDesc effectiveDesc = samplerDesc;
	const bool bUniformAddress = samplerDesc.Address.U == samplerDesc.Address.V && samplerDesc.Address.U == samplerDesc.Address.W;
	const bool bAnisotropyApplies = samplerDesc.MinMagFilter == RhiSamplerMinMagFilter::Linear && samplerDesc.MipFilter == RhiSamplerMipFilter::Linear;
	if (bUniformAddress && !bAnisotropyApplies)
	{
		effectiveDesc.MaxAnisotropy = RhiSamplerAnisotropy::X1;
	}

	const auto& samplers = GetSharedSamplerDescs();
	const auto found = std::ranges::find(samplers, effectiveDesc);
	if (found == samplers.end())
	{
		return false;
	}
	outSlot = static_cast<std::uint32_t>(found - samplers.begin());
	return true;
}

D3D12SamplerLibrary::D3D12SamplerLibrary(D3D12Rhi& rhi, RhiDescriptorService& descriptorService) :
    m_rhi(&rhi),
    m_descriptorService(&descriptorService)
{
	const auto& samplers = GetSharedSamplerDescs();
	const auto samplerCount = static_cast<std::uint32_t>(samplers.size());

	m_tableHandle = m_descriptorService->AllocateDescriptorTable(ERhiDescriptorAllocatorType::Sampler, samplerCount);
	if (!m_tableHandle)
	{
		Diagnostics::Fatal(g_samplerLibraryLogger, __FILE__, __LINE__, "Failed to allocate sampler descriptor table.");
		return;
	}

	for (std::uint32_t slot = 0; slot < samplerCount; ++slot)
	{
		CreateSampler(slot, samplers[slot]);
	}
}

D3D12SamplerLibrary::~D3D12SamplerLibrary() noexcept
{
	if (m_tableHandle && m_descriptorService)
	{
		m_descriptorService->ReleaseDescriptorTable(m_tableHandle);
		m_tableHandle = {};
	}
}

void D3D12SamplerLibrary::CreateSampler(std::uint32_t slot, const RhiSamplerDesc& samplerDesc)
{
	const std::uint32_t maxAnisotropy = static_cast<std::uint32_t>(samplerDesc.MaxAnisotropy);
	const bool bIsAnisotropic = maxAnisotropy > 1;

	D3D12_SAMPLER_DESC desc = {};
	desc.Filter = ToD3D12Filter(samplerDesc.MinMagFilter, samplerDesc.MipFilter, bIsAnisotropic);
	desc.AddressU = ToD3D12Address(samplerDesc.Address.U);
	desc.AddressV = ToD3D12Address(samplerDesc.Address.V);
	desc.AddressW = ToD3D12Address(samplerDesc.Address.W);
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = maxAnisotropy;
	desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = samplerDesc.MipFilter == RhiSamplerMipFilter::None ? 0.0f : D3D12_FLOAT32_MAX;

	const RhiCpuDescriptorHandle descriptor = m_descriptorService->GetDescriptorTableCpuHandle(m_tableHandle, slot);
	const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = D3D12TypeConversions::ToCpuDescriptor(descriptor);

	m_rhi->GetDevice()->CreateSampler(&desc, cpuHandle);
}

D3D12_FILTER D3D12SamplerLibrary::ToD3D12Filter(RhiSamplerMinMagFilter minMag, RhiSamplerMipFilter mip, bool bAnisotropic)
{
	if (bAnisotropic)
	{
		return D3D12_FILTER_ANISOTROPIC;
	}

	const uint32_t minMagBit = (minMag == RhiSamplerMinMagFilter::Linear) ? 1u : 0u;
	const uint32_t mipBit = (mip == RhiSamplerMipFilter::Linear) ? 1u : 0u;

	const uint32_t filterValue = (minMagBit << 4) | (minMagBit << 2) | mipBit;

	return static_cast<D3D12_FILTER>(filterValue);
}

D3D12_TEXTURE_ADDRESS_MODE D3D12SamplerLibrary::ToD3D12Address(RhiSamplerAddressMode address)
{
	switch (address)
	{
		case RhiSamplerAddressMode::Wrap:
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case RhiSamplerAddressMode::Clamp:
			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case RhiSamplerAddressMode::Mirror:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	}
	Diagnostics::Fatal(g_samplerLibraryLogger, __FILE__, __LINE__, "D3D12 received an unsupported sampler address mode.");
}
