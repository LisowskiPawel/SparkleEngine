#pragma once

#include "Resources/RhiResourceHandles.h"
#include "RayTracing/RhiTlasService.h"

#include <cstdint>
#include <string_view>

class VulkanGpuMemoryAllocator;
class VulkanRhi;

class VulkanTlasServices final : public RhiTlasService
{
public:
	VulkanTlasServices(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept;

	RhiRayTracingAccelerationStructurePrebuildInfo GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags = ERhiTlasBuildFlags::None) const noexcept override;
	RhiOwnedResourceHandle CreateTlasInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName) override;

private:
	VulkanRhi* m_rhi = nullptr;

	VulkanGpuMemoryAllocator* m_memoryAllocator = nullptr;
};
