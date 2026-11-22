#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <string>
#include <string_view>

enum class ShaderSourceLinePaths : std::uint8_t
{
	Virtual,
	RootRelative,
};

std::string PreprocessShaderSource(std::string_view sourcePath, const ShaderCompileRequest& request, std::vector<ShaderSourceFile>* sourceFiles = nullptr, ShaderSourceLinePaths linePaths = ShaderSourceLinePaths::Virtual);
