#pragma once

#include "../Resources/RhiResourceHandles.h"
#include "../RHIAPI.h"
#include "RhiPtlasDesc.h"

#include <string_view>

class RenderCommandList;

class SPARKLE_RHI_API RhiPtlasService
{
public:
	virtual ~RhiPtlasService() noexcept = default;

	RhiPtlasService(const RhiPtlasService&) = delete;
	RhiPtlasService& operator=(const RhiPtlasService&) = delete;
	RhiPtlasService(RhiPtlasService&&) = delete;
	RhiPtlasService& operator=(RhiPtlasService&&) = delete;

	virtual RhiPtlasBuildSizes GetPtlasBuildSizes(const RhiPtlasDesc&) const noexcept = 0;
	virtual RhiOwnedResourceHandle CreatePtlasBuffer(const RhiPtlasBuildSizes&, std::wstring_view) = 0;
	virtual RhiOwnedResourceHandle CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc&, std::wstring_view) = 0;
	virtual RhiPtlasOperationBufferLayout GetPtlasOperationBufferLayout(const RhiPtlasDesc&) const noexcept = 0;

protected:
	RhiPtlasService() noexcept = default;
};
