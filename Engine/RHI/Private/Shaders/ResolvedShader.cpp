#include "PCH.h"

#include "Shaders/ResolvedShader.h"

#include "Shaders/GlobalShaderMap.h"

ResolvedShader ResolvedShader::Resolve(const GlobalShaderMap& map, const CookedShaderLibrary& library, ShaderTypeId shaderType, ShaderTarget target) noexcept
{
	const GlobalShaderMapEntry* const entry = map.GetPublicationHash() == library.GetPublicationHash() ? map.Find(shaderType, target) : nullptr;
	const CookedShaderCodeRecord* const code = entry != nullptr ? library.Find(entry->CodeHash) : nullptr;
	return ResolvedShader{.Map = &map, .Library = &library, .Entry = entry, .Code = code};
}

bool ResolvedShader::IsValid() const noexcept
{
	return Map != nullptr && Library != nullptr && Entry != nullptr && Code != nullptr;
}

ShaderBytecode ResolvedShader::GetBytecode() const noexcept
{
	return IsValid() ? Library->GetBytecode(*Code) : ShaderBytecode{};
}
