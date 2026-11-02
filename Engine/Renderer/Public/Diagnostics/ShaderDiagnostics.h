#pragma once

#include "RHI/Public/Shaders/ShaderTarget.h"

#include <cstdint>
#include <string>
#include <vector>

struct ShaderDiagnosticsEntry final
{
	std::uint64_t ShaderType = 0;
	std::uint64_t CompileInputHash = 0;
	std::string Backend;
};

struct ShaderDiagnosticsSnapshot final
{
	std::uint64_t Generation = 0;
	ShaderTarget Target = kDefaultShaderTarget;
	std::vector<ShaderDiagnosticsEntry> Entries;
};
