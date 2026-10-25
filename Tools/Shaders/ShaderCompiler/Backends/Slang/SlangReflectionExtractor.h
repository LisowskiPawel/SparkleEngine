#pragma once

#include "ShaderReflection.h"

#include <cstddef>
#include <vector>
#include <slang.h>

#include <cstdint>
#include <string>

ShaderReflection ExtractSlangReflection(slang::ProgramLayout& programLayout, ShaderStage stage);
