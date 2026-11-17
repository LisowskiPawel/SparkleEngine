#pragma once

#include "../RHIAPI.h"
#include "../ShaderParameters/PassParameterLayout.h"
#include "ShaderStage.h"

#include <span>

struct ShaderRegistrationDesc;

SPARKLE_RHI_API ShaderStageVisibility GetDefaultShaderVisibility(ShaderStage stage) noexcept;

SPARKLE_RHI_API PassParameterLayout BuildShaderParameterLayout(const ShaderRegistrationDesc& shader);
SPARKLE_RHI_API PassParameterLayout BuildShaderPipelineParameterLayout(std::span<const ShaderRegistrationDesc* const> shaders);
