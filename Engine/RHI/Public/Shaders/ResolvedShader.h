#pragma once

#include "../RHIAPI.h"
#include "ShaderBytecode.h"
#include "ShaderMap.h"

class GlobalShaderMap;
class CookedShaderLibrary;

// The map and library must outlive this reference and any bytecode views obtained from it.
struct ResolvedShader final
{
	static SPARKLE_RHI_API ResolvedShader Resolve(const GlobalShaderMap& map, const CookedShaderLibrary& library, ShaderTypeId shaderType, ShaderTarget target) noexcept;

	const GlobalShaderMap* Map = nullptr;
	const CookedShaderLibrary* Library = nullptr;
	const GlobalShaderMapEntry* Entry = nullptr;
	const CookedShaderCodeRecord* Code = nullptr;

	SPARKLE_RHI_API bool IsValid() const noexcept;

	explicit operator bool() const noexcept { return IsValid(); }

	SPARKLE_RHI_API ShaderBytecode GetBytecode() const noexcept;
};
