#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <cstdint>

std::uint64_t HashShaderCompileRequest(const ShaderCompileRequest& request);
