#pragma once

#include "RayTracing/RhiRayTracingService.h"
#include "Vulkan/RayTracing/VulkanTlasServices.h"
#include "Vulkan/RayTracing/VulkanPtlasServices.h"
#include "Vulkan/VulkanIncludes.h"

#include <memory>
#include <cstdint>
#include <string_view>

class VulkanGpuMemoryAllocator;
class VulkanRhi;

class VulkanRayTracingServices final : public RhiRayTracingService
{
public:
	VulkanRayTracingServices(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept;

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
	std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingShaderTableDesc& desc) override;

private:
	static VkAccelerationStructureTypeKHR ToVkAccelerationStructureType(ERhiRayTracingAccelerationStructureType type) noexcept;
	static VkAccelerationStructureGeometryKHR BuildBottomLevelGeometry(const RhiRayTracingGeometryDesc& geometry) noexcept;

	VulkanRhi* m_rhi = nullptr;
	VulkanGpuMemoryAllocator* m_memoryAllocator = nullptr;
	VulkanTlasServices m_tlasServices;
	VulkanPtlasServices m_ptlasServices;
};
