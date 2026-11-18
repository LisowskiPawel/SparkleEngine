#pragma once

#include "Compiler/Output/ShaderDebugArtifactSet.h"
#include "Compiler/Output/ShaderReflection.h"

#include <cstdint>
#include <vector>

// Compiled product from one backend invocation.
class CompiledShader final
{
public:
	explicit CompiledShader(std::vector<std::uint8_t>&& bytecode);

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
