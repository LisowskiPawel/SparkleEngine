#pragma once

#include <string>
#include <string_view>

struct ShaderCompileRequest;
struct ShaderCompileJob;

std::string FormatShaderCompileRequest(const ShaderCompileRequest& request, std::string_view backendName);
std::string FormatShaderCompileJob(const ShaderCompileJob& job);
