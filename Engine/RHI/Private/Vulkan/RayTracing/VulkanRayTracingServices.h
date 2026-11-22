#pragma once

#include "RHI/Public/RayTracing/RhiRayTracingDesc.h"

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
	RhiRayTracingAccelerationStructurePrebuildInfo GetBottomLevelAccelerationStructurePrebuildInfo(const RhiRayTracingGeometryDesc& geometry) const noexcept override;
	RhiOwnedResourceHandle CreateRayTracingScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreateRayTracingAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName) override;
	std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingShaderTableDesc& desc) override;

private:
	static VkAccelerationStructureTypeKHR ToVkAccelerationStructureType(ERhiRayTracingAccelerationStructureType type) noexcept;
	static VkAccelerationStructureGeometryKHR BuildBottomLevelGeometry(const RhiRayTracingGeometryDesc& geometry) noexcept;

	VulkanRhi* m_rhi = nullptr;
	VulkanGpuMemoryAllocator* m_memoryAllocator = nullptr;
	VulkanTlasServices m_tlasServices;
	VulkanPtlasServices m_ptlasServices;
};
