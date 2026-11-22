#pragma once

#include "Cooking/Compilation/ShaderCompileJob.h"

#include <filesystem>
#include <memory>

struct ShaderMakeProducer;

void ValidateShaderMakeTree(const std::filesystem::path& root);
ShaderCompileResult ReadShaderMakeOutput(const std::filesystem::path& root, const ShaderMakeProducer& producer, std::shared_ptr<const ShaderCompileInvocation> invocation, std::string_view diagnostics);
