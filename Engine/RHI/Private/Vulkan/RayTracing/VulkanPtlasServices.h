#pragma once

#include "Resources/RhiResourceHandles.h"
#include "RayTracing/RhiPtlasOperationLayout.h"
#include "RayTracing/RhiPtlasService.h"
#include "Vulkan/VulkanIncludes.h"

#include <cstdint>
#include <string_view>

class VulkanGpuMemoryAllocator;
class VulkanRhi;

class VulkanPtlasServices final : public RhiPtlasService
{
public:
	VulkanPtlasServices(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept;

	RhiPtlasBuildSizes GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept override;
	RhiOwnedResourceHandle CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName) override;
	RhiPtlasOperationBufferLayout GetPtlasOperationBufferLayout(const RhiPtlasDesc& desc) const noexcept override;

private:
	static VkPartitionedAccelerationStructureInstanceFlagsNV ToVkPtlasInstanceFlags(RhiPtlasInstanceFlags flags) noexcept;
	static VkPartitionedAccelerationStructureOpTypeNV ToVkPtlasOperationType(ERhiPtlasOperationType type) noexcept;
	static RhiPtlasNativeOperationLayout GetNativeOperationLayout() noexcept;
	RhiGpuVirtualAddress ResolvePtlasInstanceAccelerationStructureAddress(RhiGpuVirtualAddress accelerationStructure) const noexcept;

	VulkanRhi* m_rhi = nullptr;
	VulkanGpuMemoryAllocator* m_memoryAllocator = nullptr;
};
