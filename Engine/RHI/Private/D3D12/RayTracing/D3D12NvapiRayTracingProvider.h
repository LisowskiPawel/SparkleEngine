#pragma once

#include "RayTracing/RhiPtlasDesc.h"

#include <cstdint>
#include <string_view>

struct ID3D12Device10;
struct ID3D12GraphicsCommandList7;

class D3D12NvapiRayTracingProvider final
{
public:
	D3D12NvapiRayTracingProvider() noexcept;
	~D3D12NvapiRayTracingProvider() noexcept;

	D3D12NvapiRayTracingProvider(const D3D12NvapiRayTracingProvider&) = delete;
	D3D12NvapiRayTracingProvider& operator=(const D3D12NvapiRayTracingProvider&) = delete;
	D3D12NvapiRayTracingProvider(D3D12NvapiRayTracingProvider&&) = delete;
	D3D12NvapiRayTracingProvider& operator=(D3D12NvapiRayTracingProvider&&) = delete;

	RhiPtlasCapabilities QueryPtlasCapabilities(ID3D12Device10* device, bool runsOnNvidiaDevice, bool supportsAccelerationStructure) const noexcept;
	RhiPtlasBuildSizes GetPtlasBuildSizes(ID3D12Device10* device, const RhiPtlasDesc& desc) const noexcept;
	bool BuildPtlas(ID3D12GraphicsCommandList7* commandList, const RhiPtlasBuildCommandDesc& desc) const noexcept;

	bool IsRuntimeInitialized() const noexcept;
	const char* GetRuntimeStatusReason() const noexcept;

private:
	static std::uint64_t AlignUp(std::uint64_t value, std::uint64_t alignment) noexcept;
	static const char* ToNvapiStatusReason(int status) noexcept;

	bool m_runtimeInitialized = false;
	const char* m_runtimeStatusReason = "d3d12-nvapi-headers-not-compiled";
};
