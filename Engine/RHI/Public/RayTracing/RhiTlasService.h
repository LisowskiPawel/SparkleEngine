#pragma once

#include "../Resources/RhiResourceHandles.h"
#include "../RHIAPI.h"
#include "RhiTlasDesc.h"

#include <cstdint>
#include <string_view>

class SPARKLE_RHI_API RhiTlasService
{
public:
	virtual ~RhiTlasService() noexcept = default;

	RhiTlasService(const RhiTlasService&) = delete;
	RhiTlasService& operator=(const RhiTlasService&) = delete;
	RhiTlasService(RhiTlasService&&) = delete;
	RhiTlasService& operator=(RhiTlasService&&) = delete;

	virtual RhiRayTracingAccelerationStructurePrebuildInfo GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags = ERhiTlasBuildFlags::None) const noexcept = 0;

	virtual RhiOwnedResourceHandle CreateTlasInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName) = 0;

protected:
	RhiTlasService() noexcept = default;
};
