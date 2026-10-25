#pragma once

#include "RhiAccelerationStructureDesc.h"
#include "RhiTlasDesc.h"
#include "RhiPtlasDesc.h"
#include "../RHIAPI.h"

#include <cstdint>

inline constexpr std::uint32_t kRhiRayTracingMaxPayloadSizeInBytes = 128;

enum class ERhiRayTracingTlasProvider : std::uint8_t
{
	None,
	Tlas,
	Ptlas,
};

struct RhiRayTracingProviderCapabilities
{
	ERhiRayTracingTlasProvider SelectedTlasProvider = ERhiRayTracingTlasProvider::None;
	const char* SelectedTlasProviderReason = "not-queried";
};

struct RhiRayTracingCapabilityGroups
{
	RhiTlasCapabilities Tlas;
	RhiPtlasCapabilities Ptlas;
	RhiRayTracingProviderCapabilities Provider;
};

struct RhiRayTracingCapabilities
{
	bool SupportsAccelerationStructure = false;
	bool SupportsInlineRayQuery = false;
	bool SupportsRayTracingPipeline = false;
	std::uint32_t MaxTraceRecursionDepth = 0;
	std::uint32_t MaxRayPayloadSizeInBytes = 0;
	std::uint32_t MaxRayAttributeSizeInBytes = 0;
	std::uint32_t ShaderGroupHandleSizeInBytes = 0;
	std::uint32_t ShaderTableAlignmentInBytes = 0;
	std::uint32_t ShaderTableRecordAlignmentInBytes = 0;
	std::uint32_t MaxShaderTableRecordStrideInBytes = 0;
	std::uint64_t AccelerationStructureByteAlignment = 0;
	std::uint64_t ScratchBufferByteAlignment = 0;
	std::uint32_t InstanceDescSizeInBytes = 0;
	RhiRayTracingCapabilityGroups Groups;
};

SPARKLE_RHI_API const char* RhiRayTracingTlasProviderToString(ERhiRayTracingTlasProvider provider) noexcept;
SPARKLE_RHI_API void PopulateStandardRayTracingCapabilityGroups(RhiRayTracingCapabilities& capabilities) noexcept;
