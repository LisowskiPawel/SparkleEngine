#pragma once

#include "RayTracing/RhiPtlasOperationLayout.h"
#include "RayTracing/RhiPtlasService.h"

#include <cstdint>
#include <string_view>

class D3D12GpuMemoryAllocator;
class D3D12NvapiRayTracingProvider;
class D3D12Rhi;

class D3D12PtlasServices final : public RhiPtlasService
{
public:
	D3D12PtlasServices(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator, D3D12NvapiRayTracingProvider& nvapiProvider) noexcept;

	RhiPtlasBuildSizes GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept override;
	RhiOwnedResourceHandle CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName) override;
	RhiPtlasOperationBufferLayout GetPtlasOperationBufferLayout(const RhiPtlasDesc& desc) const noexcept override;

private:
	static std::uint32_t ToNvapiPtlasInstanceFlags(RhiPtlasInstanceFlags flags) noexcept;
	static std::uint32_t ToNvapiPtlasOperationType(ERhiPtlasOperationType type) noexcept;
	static RhiPtlasNativeOperationLayout GetNativeOperationLayout() noexcept;

	D3D12Rhi* m_rhi = nullptr;
	D3D12GpuMemoryAllocator* m_memoryAllocator = nullptr;
	D3D12NvapiRayTracingProvider* m_nvapiProvider = nullptr;
};
