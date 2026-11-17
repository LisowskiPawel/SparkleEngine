#include "PCH.h"

#include "CompiledShader.h"

#include <utility>

CompiledShader::CompiledShader(std::vector<std::uint8_t>&& bytecode) :
    m_bytecode(std::move(bytecode))
{
}

std::vector<std::uint8_t> CompiledShader::TakeBytecode() noexcept
{
	return std::move(m_bytecode);
}

ShaderReflection CompiledShader::TakeReflection() noexcept
{
	return std::move(m_reflection);
}

ShaderDebugArtifactSet CompiledShader::TakeDebugArtifacts() noexcept
{
	return std::move(m_debugArtifacts);
}

void CompiledShader::SetReflection(ShaderReflection&& reflection) noexcept
{
	m_reflection = std::move(reflection);
}

void CompiledShader::SetDebugArtifacts(ShaderDebugArtifactSet&& debugArtifacts) noexcept
{
	m_debugArtifacts = std::move(debugArtifacts);
}
