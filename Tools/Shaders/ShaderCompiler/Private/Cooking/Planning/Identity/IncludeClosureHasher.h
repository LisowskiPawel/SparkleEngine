#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

struct IncludeClosureHash final
{
	std::uint64_t sourceHash = 0;
	std::uint64_t includeClosureHash = 0;
	std::vector<std::string> virtualDependencies;
};

IncludeClosureHash HashShaderIncludeClosure(const ShaderCompileRequest& request);
