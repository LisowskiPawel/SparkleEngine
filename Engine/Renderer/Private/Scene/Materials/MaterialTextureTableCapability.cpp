#include "PCH.h"
#include "Scene/Materials/MaterialTextureTableCapability.h"

#include <algorithm>

bool SupportsMaterialTextureTable(const RhiCapabilities& capabilities) noexcept
{
	if (capabilities.BackendApi == ERhiBackendApi::Unknown || capabilities.DescriptorModel == ERhiDescriptorModel::Unknown)
	{
		return false;
	}
	if (!capabilities.DescriptorIndexing.SupportsSampledImageArrayNonUniformIndexing)
	{
		return false;
	}
	if (!capabilities.DescriptorIndexing.SupportsPartiallyBoundDescriptorArrays)
	{
		return false;
	}

	const RhiBindingLimits& limits = capabilities.BindingLimits;
	std::uint32_t capacity = limits.MaxShaderResourceDescriptors;
	if (limits.MaxDescriptorTableEntries != 0)
	{
		capacity = capacity == 0 ? limits.MaxDescriptorTableEntries : std::min(capacity, limits.MaxDescriptorTableEntries);
	}
	return capacity >= MaterialTextureTableFixedCapacity;
}
