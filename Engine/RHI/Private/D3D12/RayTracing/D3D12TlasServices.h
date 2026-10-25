#pragma once

#include "RayTracing/RhiTlasService.h"

#include <cstdint>
#include <string_view>

class D3D12GpuMemoryAllocator;
class D3D12Rhi;

class D3D12TlasServices final : public RhiTlasService
{
public:
	D3D12TlasServices(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator) noexcept;

	RhiRayTracingAccelerationStructurePrebuildInfo GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags = ERhiTlasBuildFlags::None) const noexcept override;
	RhiOwnedResourceHandle CreateTlasInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName) override;

private:
	D3D12Rhi* m_rhi = nullptr;

	D3D12GpuMemoryAllocator* m_memoryAllocator = nullptr;
};
