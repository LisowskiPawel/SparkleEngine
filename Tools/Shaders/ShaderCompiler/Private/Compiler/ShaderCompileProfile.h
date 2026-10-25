#pragma once

#include "Compiler/ShaderCompileRequest.h"

#include <string>

// Fixed SPIR-V ABI shared by compile identity and every backend's native option mapping.
inline constexpr bool SpirVUseDirectXBufferLayout = true;
inline constexpr bool SpirVUseUnknownStorageImageFormat = true;

const char* GetShaderModelProfileName(ShaderTarget target);
const char* GetSpirVProfileName(ShaderTarget target);
const char* GetSlangTargetProfileName(ShaderTarget target);
std::string BuildShaderTargetProfile(const ShaderCompileRequest& request);
