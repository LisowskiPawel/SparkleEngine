#include "PCH.h"
#include "D3D12/Samplers/D3D12SamplerLibrary.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/D3D12TypeConversions.h"

#include <algorithm>
#include <array>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_samplerLibraryLogger, "RHI.D3D12.Samplers");

bool D3D12SamplerLibrary::TryGetSlot(const RhiSamplerDesc& samplerDesc, Slot& outSlot) noexcept
{
	const RhiSamplerDesc linearNoMipWrapClampClamp{
	    .MinMagFilter = RhiSamplerMinMagFilter::Linear,
	    .MipFilter = RhiSamplerMipFilter::None,
	    .Address = {.U = RhiSamplerAddressMode::Wrap, .V = RhiSamplerAddressMode::Clamp, .W = RhiSamplerAddressMode::Clamp}};

	if (samplerDesc == linearNoMipWrapClampClamp)
	{
		outSlot = Slot::LinearNoMipWrapClampClamp;
		return true;
	}

	RhiSamplerAddressMode addressMode = RhiSamplerAddressMode::Wrap;
	if (!TryGetUniformAddressMode(samplerDesc, addressMode))
	{
		return false;
	}

	if (samplerDesc.MaxAnisotropy != RhiSamplerAnisotropy::X1)
	{
		const bool bAnisotropyApplies = samplerDesc.MinMagFilter == RhiSamplerMinMagFilter::Linear && samplerDesc.MipFilter == RhiSamplerMipFilter::Linear;
		if (bAnisotropyApplies)
		{
			return TryGetAnisotropicSlot(samplerDesc.MaxAnisotropy, addressMode, outSlot);
		}
	}

	return TryGetFilteredSlot(samplerDesc.MinMagFilter, samplerDesc.MipFilter, addressMode, outSlot);
}

D3D12SamplerLibrary::D3D12SamplerLibrary(D3D12Rhi& rhi, RhiDescriptorService& descriptorService) :
    m_rhi(&rhi),
    m_descriptorService(&descriptorService)
{
	constexpr uint32_t samplerCount = static_cast<uint32_t>(Slot::Count);

	m_tableHandle = m_descriptorService->AllocateDescriptorTable(ERhiDescriptorAllocatorType::Sampler, samplerCount);
	if (!m_tableHandle)
	{
		Diagnostics::Fatal(g_samplerLibraryLogger, __FILE__, __LINE__, "Failed to allocate sampler descriptor table.");
		return;
	}

	CreateSharedSamplers();
	m_bInitialized = true;
}

void D3D12SamplerLibrary::CreateSharedSamplers()
{
	const auto createSampler = [this](Slot slot, RhiSamplerMinMagFilter minMagFilter, RhiSamplerMipFilter mipFilter, RhiSamplerAddressMode addressMode, RhiSamplerAnisotropy maxAnisotropy)
	{
		CreateSampler(slot, MakeSamplerDesc(minMagFilter, mipFilter, addressMode, maxAnisotropy));
	};

	createSampler(Slot::PointMipPointWrap, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::Point, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointMipPointClamp, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::Point, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointMipPointMirror, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::Point, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointMipLinearWrap, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointMipLinearClamp, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointMipLinearMirror, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointNoMipWrap, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::None, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointNoMipClamp, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::None, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X1);
	createSampler(Slot::PointNoMipMirror, RhiSamplerMinMagFilter::Point, RhiSamplerMipFilter::None, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X1);

	createSampler(Slot::LinearMipPointWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Point, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearMipPointClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Point, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearMipPointMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Point, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearMipLinearWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearMipLinearClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearMipLinearMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearNoMipWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::None, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearNoMipClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::None, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X1);
	createSampler(Slot::LinearNoMipMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::None, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X1);

	CreateSampler(
	    Slot::LinearNoMipWrapClampClamp,
	    RhiSamplerDesc{
	        .MinMagFilter = RhiSamplerMinMagFilter::Linear,
	        .MipFilter = RhiSamplerMipFilter::None,
	        .Address = {.U = RhiSamplerAddressMode::Wrap, .V = RhiSamplerAddressMode::Clamp, .W = RhiSamplerAddressMode::Clamp}});

	createSampler(Slot::Aniso1xWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X1);
	createSampler(Slot::Aniso1xClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X1);
	createSampler(Slot::Aniso1xMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X1);
	createSampler(Slot::Aniso2xWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X2);
	createSampler(Slot::Aniso2xClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X2);
	createSampler(Slot::Aniso2xMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X2);
	createSampler(Slot::Aniso4xWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X4);
	createSampler(Slot::Aniso4xClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X4);
	createSampler(Slot::Aniso4xMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X4);
	createSampler(Slot::Aniso8xWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X8);
	createSampler(Slot::Aniso8xClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X8);
	createSampler(Slot::Aniso8xMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X8);
	createSampler(Slot::Aniso16xWrap, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Wrap, RhiSamplerAnisotropy::X16);
	createSampler(Slot::Aniso16xClamp, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Clamp, RhiSamplerAnisotropy::X16);
	createSampler(Slot::Aniso16xMirror, RhiSamplerMinMagFilter::Linear, RhiSamplerMipFilter::Linear, RhiSamplerAddressMode::Mirror, RhiSamplerAnisotropy::X16);
}

D3D12SamplerLibrary::~D3D12SamplerLibrary() noexcept
{
	if (m_tableHandle && m_descriptorService)
	{
		m_descriptorService->ReleaseDescriptorTable(m_tableHandle);
		m_tableHandle = {};
	}
	m_bInitialized = false;
}

void D3D12SamplerLibrary::CreateSampler(Slot slot, const RhiSamplerDesc& samplerDesc)
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

	const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = D3D12TypeConversions::ToCpuDescriptor(m_descriptorService->GetDescriptorTableCpuHandle(m_tableHandle, static_cast<uint32_t>(slot)));

	m_rhi->GetDevice()->CreateSampler(&desc, cpuHandle);
}

RhiSamplerDesc D3D12SamplerLibrary::MakeSamplerDesc(RhiSamplerMinMagFilter minMagFilter, RhiSamplerMipFilter mipFilter, RhiSamplerAddressMode addressMode, RhiSamplerAnisotropy maxAnisotropy) noexcept
{
	return RhiSamplerDesc{.MinMagFilter = minMagFilter, .MipFilter = mipFilter, .Address = MakeRhiSamplerAddressModes(addressMode), .MaxAnisotropy = maxAnisotropy};
}

bool D3D12SamplerLibrary::TryGetAddressOffset(RhiSamplerAddressMode addressMode, std::uint32_t& outOffset) noexcept
{
	static_assert(
	    static_cast<std::uint32_t>(RhiSamplerAddressMode::Wrap) == 0 && static_cast<std::uint32_t>(RhiSamplerAddressMode::Clamp) == 1
	    && static_cast<std::uint32_t>(RhiSamplerAddressMode::Mirror) == 2);

	const std::uint32_t offset = static_cast<std::uint32_t>(addressMode);
	if (offset > 2)
	{
		return false;
	}
	outOffset = offset;
	return true;
}

bool D3D12SamplerLibrary::TryGetUniformAddressMode(const RhiSamplerDesc& samplerDesc, RhiSamplerAddressMode& outAddressMode) noexcept
{
	if (samplerDesc.Address.U != samplerDesc.Address.V || samplerDesc.Address.U != samplerDesc.Address.W)
	{
		return false;
	}

	outAddressMode = samplerDesc.Address.U;
	return true;
}

bool D3D12SamplerLibrary::TryGetFilteredSlot(RhiSamplerMinMagFilter minMagFilter, RhiSamplerMipFilter mipFilter, RhiSamplerAddressMode addressMode, Slot& outSlot) noexcept
{
	static_assert(static_cast<std::uint32_t>(RhiSamplerMinMagFilter::Point) == 0 && static_cast<std::uint32_t>(RhiSamplerMinMagFilter::Linear) == 1);

	static_assert(
	    static_cast<std::uint32_t>(RhiSamplerMipFilter::None) == 0 && static_cast<std::uint32_t>(RhiSamplerMipFilter::Point) == 1 && static_cast<std::uint32_t>(RhiSamplerMipFilter::Linear) == 2);

	constexpr std::array baseSlots{
	    std::array{Slot::PointNoMipWrap, Slot::PointMipPointWrap, Slot::PointMipLinearWrap},
	    std::array{Slot::LinearNoMipWrap, Slot::LinearMipPointWrap, Slot::LinearMipLinearWrap}};

	const std::uint32_t filterIndex = static_cast<std::uint32_t>(minMagFilter);
	const std::uint32_t mipIndex = static_cast<std::uint32_t>(mipFilter);
	std::uint32_t addressOffset = 0;
	if (filterIndex >= baseSlots.size() || mipIndex >= baseSlots[0].size() || !TryGetAddressOffset(addressMode, addressOffset))
	{
		return false;
	}
	outSlot = static_cast<Slot>(static_cast<std::uint32_t>(baseSlots[filterIndex][mipIndex]) + addressOffset);
	return true;
}

bool D3D12SamplerLibrary::TryGetAnisotropicSlot(RhiSamplerAnisotropy maxAnisotropy, RhiSamplerAddressMode addressMode, Slot& outSlot) noexcept
{
	std::uint32_t addressOffset = 0;
	if (!TryGetAddressOffset(addressMode, addressOffset))
	{
		return false;
	}

	constexpr std::array baseSlots{
	    std::pair{RhiSamplerAnisotropy::X1, Slot::Aniso1xWrap},
	    std::pair{RhiSamplerAnisotropy::X2, Slot::Aniso2xWrap},
	    std::pair{RhiSamplerAnisotropy::X4, Slot::Aniso4xWrap},
	    std::pair{RhiSamplerAnisotropy::X8, Slot::Aniso8xWrap},
	    std::pair{RhiSamplerAnisotropy::X16, Slot::Aniso16xWrap}};

	const auto found = std::find_if(baseSlots.begin(), baseSlots.end(), [maxAnisotropy](const auto& entry) { return entry.first == maxAnisotropy; });
	if (found == baseSlots.end())
	{
		return false;
	}
	outSlot = static_cast<Slot>(static_cast<std::uint32_t>(found->second) + addressOffset);
	return true;
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
