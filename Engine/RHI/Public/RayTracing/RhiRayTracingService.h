#pragma once

#include "RhiAccelerationStructureDesc.h"
#include "../RayTracing/RhiRayTracingPipelineDesc.h"
#include "../Resources/RhiResourceHandles.h"
#include "../RHIAPI.h"

#include <cstdint>
#include <memory>
#include <string_view>

class RhiTlasService;
class RhiPtlasService;

class SPARKLE_RHI_API RhiRayTracingService
{
public:
	virtual ~RhiRayTracingService() noexcept = default;

	RhiRayTracingService(const RhiRayTracingService&) = delete;
	RhiRayTracingService& operator=(const RhiRayTracingService&) = delete;
	RhiRayTracingService(RhiRayTracingService&&) = delete;
	RhiRayTracingService& operator=(RhiRayTracingService&&) = delete;

	virtual RhiTlasService& GetTlasService() noexcept = 0;
	virtual const RhiTlasService& GetTlasService() const noexcept = 0;
	virtual RhiPtlasService& GetPtlasService() noexcept = 0;
	virtual const RhiPtlasService& GetPtlasService() const noexcept = 0;
	virtual RhiRayTracingAccelerationStructurePrebuildInfo GetBottomLevelAccelerationStructurePrebuildInfo(const RhiRayTracingGeometryDesc& geometry) const noexcept = 0;
	virtual RhiOwnedResourceHandle CreateRayTracingScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName) = 0;
	virtual RhiOwnedResourceHandle CreateRayTracingAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName) = 0;
	virtual std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingShaderTableDesc& desc) = 0;

protected:
	RhiRayTracingService() noexcept = default;
};
