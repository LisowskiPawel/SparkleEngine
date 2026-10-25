#pragma once

#include "RhiAccelerationStructureDesc.h"

#include <array>
#include <cstdint>

struct RhiTlasCapabilities
{
	bool SupportsTlasBuild = false;
	bool SupportsTlasUpdate = false;
	bool SupportsGpuReadableInstanceBuffer = false;
	std::uint32_t InstanceDescSizeInBytes = 0;
};

enum class ERhiTlasBuildFlags : std::uint8_t
{
	None = 0,
	AllowUpdate = 1 << 0,
};

enum class ERhiTlasBuildMode : std::uint8_t
{
	Build,
	BuildAllowUpdate,
	Update,
};

enum class RhiRayTracingInstanceFlags : std::uint8_t
{
	None = 0,
	TriangleFacingCullDisable = 1 << 0,
	ForceOpaque = 1 << 1,
	ForceNonOpaque = 1 << 2,
};

constexpr ERhiTlasBuildFlags operator|(ERhiTlasBuildFlags lhs, ERhiTlasBuildFlags rhs) noexcept
{
	return static_cast<ERhiTlasBuildFlags>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

constexpr bool HasFlag(ERhiTlasBuildFlags flags, ERhiTlasBuildFlags flag) noexcept
{
	return (static_cast<std::uint8_t>(flags) & static_cast<std::uint8_t>(flag)) != 0;
}

constexpr RhiRayTracingInstanceFlags operator|(RhiRayTracingInstanceFlags lhs, RhiRayTracingInstanceFlags rhs) noexcept
{
	return static_cast<RhiRayTracingInstanceFlags>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

constexpr bool HasFlag(RhiRayTracingInstanceFlags flags, RhiRayTracingInstanceFlags flag) noexcept
{
	return (static_cast<std::uint8_t>(flags) & static_cast<std::uint8_t>(flag)) != 0;
}

struct RhiRayTracingInstanceDesc
{
	std::array<float, 12> Transform = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
	std::uint32_t InstanceID = 0;
	std::uint32_t InstanceMask = 0xFF;
	std::uint32_t InstanceContributionToHitGroupIndex = 0;
	RhiRayTracingInstanceFlags Flags = RhiRayTracingInstanceFlags::TriangleFacingCullDisable;
	RhiGpuVirtualAddress AccelerationStructure = 0;
};
