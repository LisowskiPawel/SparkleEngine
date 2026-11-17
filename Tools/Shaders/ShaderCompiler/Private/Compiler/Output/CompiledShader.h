#pragma once

#include "RHI/Public/Shaders/ShaderBytecode.h"
#include "Compiler/Output/ShaderDebugArtifactSet.h"
#include "Compiler/Output/ShaderReflection.h"

#include <cstdint>
#include <vector>

// Compiled product from one backend invocation.
// ShaderBytecode stays under RHI public because runtime readers return it.
class CompiledShader final
{
public:
	explicit CompiledShader(std::vector<std::uint8_t>&& bytecode);

	ShaderBytecode GetBytecode() const noexcept { return {m_bytecode.data(), m_bytecode.size()}; }

	std::vector<std::uint8_t> TakeBytecode() noexcept;
	ShaderReflection TakeReflection() noexcept;
	ShaderDebugArtifactSet TakeDebugArtifacts() noexcept;

	void SetReflection(ShaderReflection&& reflection) noexcept;
	void SetDebugArtifacts(ShaderDebugArtifactSet&& debugArtifacts) noexcept;

private:
	std::vector<std::uint8_t> m_bytecode;
	ShaderReflection m_reflection;
	ShaderDebugArtifactSet m_debugArtifacts;
};
