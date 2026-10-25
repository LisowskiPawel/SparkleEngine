#pragma once

#include "Pipeline/RhiPipelineDesc.h"

#include <span>
#include <string_view>
#include <vector>

class PassParameterLayout;

struct RhiReflectedBindingLocation final
{
	RhiBindingPoint BindingPoint = {};
	ShaderStageMask VisibilityMask = ShaderStageMask::None;
};

std::vector<RhiReflectedBindingLocation> ResolveShaderBindingLocations(
    std::span<const ResolvedShader> shaders,
    const PassParameterLayout& parameterLayout,
    std::string_view bindingName,
    ShaderParameterSemanticKind semanticKind);
