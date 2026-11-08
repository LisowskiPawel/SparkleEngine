#pragma once

#include "RayTracing/RhiPtlasOperationLayout.h"
#include "RayTracing/RhiPtlasDesc.h"

#include <cstdint>
#include <vector>

namespace D3D12PtlasOperationPacking
{
	RhiPtlasNativeOperationLayout GetNativeLayout() noexcept;
	std::vector<std::uint8_t> Pack(const RhiPtlasOperationPackDesc& operationPack, const RhiPtlasOperationBufferLayout& layout, RhiGpuVirtualAddress baseAddress);
}
