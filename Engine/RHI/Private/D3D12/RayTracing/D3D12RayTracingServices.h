#pragma once

#include "D3D12/RayTracing/D3D12TlasServices.h"
#include "D3D12/RayTracing/D3D12NvapiRayTracingProvider.h"
#include "D3D12/RayTracing/D3D12PtlasServices.h"
#include "RayTracing/RhiRayTracingService.h"

#include <memory>
#include <cstdint>
#include <string_view>

class D3D12GpuMemoryAllocator;
class D3D12Rhi;
class D3D12ResourceService;
struct ID3D12GraphicsCommandList7;

class D3D12RayTracingServices final : public RhiRayTracingService
{
public:
	D3D12RayTracingServices(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator, D3D12ResourceService& resourceService, D3D12NvapiRayTracingProvider& nvapiProvider) noexcept;

	RhiTlasService& GetTlasService() noexcept override;
	const RhiTlasService& GetTlasService() const noexcept override;
	RhiPtlasService& GetPtlasService() noexcept override;
	const RhiPtlasService& GetPtlasService() const noexcept override;
	RhiRayTracingCapabilities GetCapabilities() const noexcept;
	RhiRayTracingAccelerationStructurePrebuildInfo GetBottomLevelAccelerationStructurePrebuildInfo(const RhiRayTracingGeometryDesc& geometry) const noexcept override;
	RhiRayTracingAccelerationStructurePrebuildInfo GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags = ERhiTlasBuildFlags::None) const noexcept override;
	RhiPtlasBuildSizes GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept override;
	RhiOwnedResourceHandle CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreateScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName);
	RhiOwnedResourceHandle CreateRayTracingScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreateAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName);
	RhiOwnedResourceHandle CreateRayTracingAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreateInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName);
	RhiOwnedResourceHandle CreateRayTracingInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName) override;
	bool BuildPtlas(ID3D12GraphicsCommandList7* commandList, const RhiPtlasBuildCommandDesc& desc) const noexcept;
	std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingShaderTableDesc& desc) override;

private:
	D3D12Rhi* m_rhi = nullptr;

	D3D12GpuMemoryAllocator* m_memoryAllocator = nullptr;
	D3D12ResourceService* m_resourceService = nullptr;
	D3D12NvapiRayTracingProvider* m_nvapiProvider = nullptr;
	D3D12TlasServices m_tlasServices;
	D3D12PtlasServices m_ptlasServices;
};
