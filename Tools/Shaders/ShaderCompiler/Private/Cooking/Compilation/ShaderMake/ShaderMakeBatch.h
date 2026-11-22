#pragma once

#include "Cooking/Compilation/ShaderCompileJob.h"
#include "ToolWorkProgressEvent.h"

#include <cstddef>
#include <span>
#include <vector>

struct ShaderCookSettings;

std::vector<ShaderCompileResult> CompileShaderMakeProducers(
    const ShaderCookSettings& settings,
    std::span<const ShaderCompileJob> jobs,
    std::span<const std::size_t> producerIndices,
    std::size_t totalWork,
    const ToolWorkProgressCallback& progress);
