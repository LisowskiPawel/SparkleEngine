#pragma once

#include "RHI/Public/Core/RhiCapabilities.h"

#include <cstdint>

inline constexpr std::uint32_t MaterialTextureTableFixedCapacity = 4096u;

bool SupportsMaterialTextureTable(const RhiCapabilities& capabilities) noexcept;
