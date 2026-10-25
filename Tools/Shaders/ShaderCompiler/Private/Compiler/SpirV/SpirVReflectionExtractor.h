#pragma once

#include "ShaderReflection.h"

#include <vector>
#include <cstdint>
#include <span>
#include <string>


// SPIR-V -> ShaderReflection extractor backed by SPIRV-Reflect.
// SPIRV-Reflect symbols stay confined to the compiler's SPIR-V implementation.
ShaderReflection ExtractSpirVReflection(std::span<const std::uint8_t> bytecode, ShaderStage stage);
