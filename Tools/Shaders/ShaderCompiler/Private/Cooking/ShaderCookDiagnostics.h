#pragma once

#include "RHI/Public/Shaders/ShaderTarget.h"
#include "Cooking/Compilation/ShaderCompileJob.h"

#include <string>
#include <string_view>

std::string FormatShaderCompileJob(const ShaderCompileJob& job, std::string_view backendName, ShaderTarget target);
