#pragma once

#include "ToolWorkProgressEvent.h"

#include <cstddef>
#include <span>
#include <vector>

struct ShaderCookSettings;
struct ShaderCompileJob;
struct ShaderCompileResult;

std::vector<ShaderCompileResult> CompileShaderBatch(const ShaderCookSettings& settings, std::span<const ShaderCompileJob> jobs, std::size_t totalWork, const ToolWorkProgressCallback& progress);
