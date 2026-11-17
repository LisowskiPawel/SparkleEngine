#include "PCH.h"

#include "Shaders/GlobalShaderMap.h"

bool ResolvedShader::IsValid() const noexcept
{
	return Map != nullptr && Library != nullptr && Entry != nullptr && Code != nullptr;
}

ShaderBytecode ResolvedShader::GetBytecode() const noexcept
{
	return IsValid() ? Library->GetBytecode(*Code) : ShaderBytecode{};
}
