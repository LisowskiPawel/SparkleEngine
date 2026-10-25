#include "PCH.h"

#include "RayTracing/RhiPtlasDesc.h"

#include <cstdint>

RhiPtlasInstanceFlags operator|(RhiPtlasInstanceFlags lhs, RhiPtlasInstanceFlags rhs) noexcept
{
	return static_cast<RhiPtlasInstanceFlags>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
}

bool HasFlag(RhiPtlasInstanceFlags flags, RhiPtlasInstanceFlags flag) noexcept
{
	return (static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(flag)) != 0;
}

const char* RhiPtlasProviderToString(ERhiPtlasProvider provider) noexcept
{
	switch (provider)
	{
		case ERhiPtlasProvider::VulkanNvPtlas:
			return "VulkanNvPtlas";
		case ERhiPtlasProvider::D3D12NvapiPtlas:
			return "D3D12NvapiPtlas";
		case ERhiPtlasProvider::D3D12PublicDxrRtasOperations:
			return "D3D12PublicDxrRtasOperations";
		case ERhiPtlasProvider::None:
		default:
			return "None";
	}
}
