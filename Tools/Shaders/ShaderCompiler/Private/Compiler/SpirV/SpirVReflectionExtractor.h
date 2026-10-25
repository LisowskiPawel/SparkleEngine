#pragma once

#include "ShaderReflection.h"

#include <cstdint>
#include <span>

// SPIR-V -> ShaderReflection extractor backed by SPIRV-Reflect.
// SPIRV-Reflect symbols stay confined to the compiler's SPIR-V implementation.
ShaderReflection ExtractSpirVReflection(std::span<const std::uint8_t> bytecode, ShaderStage stage);
