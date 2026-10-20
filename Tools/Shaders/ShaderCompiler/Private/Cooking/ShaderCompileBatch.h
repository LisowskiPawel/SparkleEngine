#pragma once

#include "ToolWorkProgressEvent.h"

#include <cstddef>
#include <span>
#include <vector>

struct ShaderCookSettings;
struct ShaderCompileJob;
struct ShaderCompileResult;

namespace ShaderCompileBatch
{
	std::vector<ShaderCompileResult> Execute(const ShaderCookSettings& settings, std::span<const ShaderCompileJob> jobs, std::size_t totalWork, const ToolWorkProgressCallback& progress);
}
