#pragma once

#include "ShaderParameters/ShaderParameterStruct.h"
#include "RHI/Public/Shaders/Authoring/GlobalShader.h"

class ChromaticAberrationCS final : public GlobalShader<ChromaticAberrationCS>
{
public:
	BEGIN_SHADER_PARAMETER_STRUCT(Parameters, ChromaticAberrationCS)
	SHADER_PARAMETER_TEXTURE_SRV(Texture2D, DisplayLinearColor)
	SHADER_PARAMETER_TEXTURE_UAV(RWTexture2D, OutputColor)
	END_SHADER_PARAMETER_STRUCT()
};
