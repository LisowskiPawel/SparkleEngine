#pragma once

#include <cstdint>

enum class EngineRayReconstructionMode : std::uint8_t
{
	Off,
	NvidiaDlssRayReconstruction,
};

constexpr bool IsValidConsoleVariableValue(EngineRayReconstructionMode value) noexcept
{
	return value == EngineRayReconstructionMode::Off || value == EngineRayReconstructionMode::NvidiaDlssRayReconstruction;
}
