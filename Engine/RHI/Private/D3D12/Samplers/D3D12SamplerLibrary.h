#pragma once

#include "Descriptors/RhiDescriptorService.h"
#include <d3d12.h>

#include <cstdint>

class D3D12Rhi;

class D3D12SamplerLibrary
{
public:
	D3D12SamplerLibrary(D3D12Rhi& rhi, RhiDescriptorService& descriptorService);
	~D3D12SamplerLibrary() noexcept;

	D3D12SamplerLibrary(const D3D12SamplerLibrary&) = delete;
	D3D12SamplerLibrary& operator=(const D3D12SamplerLibrary&) = delete;
	D3D12SamplerLibrary(D3D12SamplerLibrary&&) = delete;
	D3D12SamplerLibrary& operator=(D3D12SamplerLibrary&&) = delete;

	RhiDescriptorTableHandle GetTableHandle() const noexcept { return m_tableHandle; }

	static bool TryGetSlot(const RhiSamplerDesc& samplerDesc, std::uint32_t& outSlot) noexcept;

private:
	void CreateSampler(std::uint32_t slot, const RhiSamplerDesc& desc);
	static D3D12_FILTER ToD3D12Filter(RhiSamplerMinMagFilter minMag, RhiSamplerMipFilter mip, bool bAnisotropic);
	static D3D12_TEXTURE_ADDRESS_MODE ToD3D12Address(RhiSamplerAddressMode address);

	D3D12Rhi* m_rhi = nullptr;
	RhiDescriptorService* m_descriptorService = nullptr;
	RhiDescriptorTableHandle m_tableHandle = {};
};
