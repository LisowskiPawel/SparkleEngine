#pragma once

#include "RayTracing/RhiPtlasOperationLayout.h"
#include "RayTracing/RhiPtlasDesc.h"

#include <cstdint>
#include <vector>

class VulkanGpuMemoryAllocator;

namespace VulkanPtlasOperationPacking
{
	RhiPtlasNativeOperationLayout GetNativeLayout() noexcept;
	std::vector<std::uint8_t> Pack(
	    const RhiPtlasOperationPackDesc& operationPack,
	    const RhiPtlasOperationBufferLayout& layout,
	    RhiGpuVirtualAddress baseAddress,
	    const VulkanGpuMemoryAllocator& memoryAllocator);
}
