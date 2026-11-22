#pragma once

#include "Compiler/Output/ShaderReflection.h"

#include <string_view>

ShaderReflection ReadSlangReflection(std::string_view input, ShaderStage stage, std::string_view entryPoint);
