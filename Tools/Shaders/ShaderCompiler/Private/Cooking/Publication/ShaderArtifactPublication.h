#pragma once

#include "Cooking/ShaderCookContext.h"
#include "Cooking/ShaderCookOutput.h"

#include <filesystem>
#include <span>

struct ShaderCookSettings;

void PrepareShaderProducts(ShaderCookPipelinePlan& plan, std::span<ShaderCompileResult> compileResults);

ShaderCookOutput PublishShaderArtifacts(
    const ShaderCookPipelinePlan& plan,
    std::span<const ShaderCompileResult> compileResults,
    const ShaderCookSettings& settings,
    const std::filesystem::path& outputDirectory);
