#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <cstdint>
#include <span>
#include <vector>

void NormalizeSpirVBindings(std::vector<std::uint8_t>& bytecode, std::span<const ShaderDescriptorBindingRemap> remaps);
