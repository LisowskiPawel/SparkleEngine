#pragma once

#include "Compiler/Output/ShaderReflection.h"
#include "Shaders/Authoring/ShaderParameterStruct.h"

#include <string>
#include <vector>

struct ShaderParameterStructVerificationResult final
{
	std::vector<std::string> mismatches;
	std::vector<std::string> diagnostics;
};

ShaderParameterStructVerificationResult VerifyShaderParameterStruct(const ShaderParameterStructDescriptor& descriptor, const ShaderReflection& reflection, bool bAllowUnreflectedDeclarations = false);
