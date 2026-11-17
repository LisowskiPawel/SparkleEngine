#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <string>
#include <string_view>

std::string PreprocessShaderSource(std::string_view sourcePath, const ShaderCompileRequest& request, std::vector<ShaderSourceFile>* sourceFiles = nullptr);
