#pragma once

#include "../Memory/RhiMemoryTypes.h"
#include "../RayTracing/RhiTlasService.h"
#include "../RayTracing/RhiPtlasService.h"
#include "../RayTracing/RhiRayTracingDesc.h"
#include "../RayTracing/RhiRayTracingPipelineDesc.h"
#include "../Resources/RhiResourceDesc.h"
#include "../Resources/RhiResourceHandles.h"
#include "../RHIAPI.h"

#include <cstdint>
#include <memory>
#include <string_view>

class RenderCommandList;

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
	virtual RhiRayTracingAccelerationStructurePrebuildInfo GetTlasPrebuildInfo(std::uint32_t instanceCount, ERhiTlasBuildFlags buildFlags = ERhiTlasBuildFlags::None) const noexcept;
	virtual RhiPtlasBuildSizes GetPtlasBuildSizes(const RhiPtlasDesc& desc) const noexcept;
	virtual RhiOwnedResourceHandle CreatePtlasBuffer(const RhiPtlasBuildSizes& sizes, std::wstring_view debugName);
	virtual RhiOwnedResourceHandle CreatePtlasOperationBuffer(const RhiPtlasOperationPackDesc& operationPack, std::wstring_view debugName);
	virtual RhiPtlasOperationBufferLayout GetPtlasOperationBufferLayout(const RhiPtlasDesc& desc) const noexcept;
	virtual RhiOwnedResourceHandle CreateRayTracingScratchBuffer(std::uint64_t sizeInBytes, std::wstring_view debugName) = 0;
	virtual RhiOwnedResourceHandle CreateRayTracingAccelerationStructureBuffer(std::uint64_t sizeInBytes, ERhiRayTracingAccelerationStructureType type, std::wstring_view debugName) = 0;
	virtual RhiOwnedResourceHandle CreateRayTracingInstanceBuffer(const RhiRayTracingInstanceDesc* instances, std::uint32_t instanceCount, std::wstring_view debugName);
	virtual std::unique_ptr<RayTracingShaderTable> CreateRayTracingShaderTable(const RayTracingShaderTableDesc& desc) = 0;

protected:
	RhiRayTracingService() noexcept = default;
};
