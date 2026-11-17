#pragma once

#include "Compiler/Output/ShaderReflection.h"

#include <vector>
#include <slang.h>

ShaderReflection ExtractSlangReflection(slang::ProgramLayout& programLayout, ShaderStage stage);
