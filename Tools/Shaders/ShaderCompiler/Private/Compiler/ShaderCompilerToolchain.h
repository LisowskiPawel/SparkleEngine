#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct ShaderCompilerInputFile final
{
	std::string Name;
	std::string Sha256;
};

struct ShaderCompilerToolchain final
{
	std::uint64_t Version = 0;
	std::vector<ShaderCompilerInputFile> Files;
};

ShaderCompilerToolchain ReadShaderCompilerToolchain();
