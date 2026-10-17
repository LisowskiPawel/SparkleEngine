#pragma once

#include "RHI/Public/Shaders/ShaderBytecode.h"
#include "ShaderDebugArtifactSet.h"
#include "ShaderReflection.h"

#include <cstdint>
#include <utility>
#include <vector>

// Compiled product from one backend invocation.
// ShaderBytecode stays under RHI public because runtime readers return it.
class CompiledShader
{
public:
	explicit CompiledShader(std::vector<std::uint8_t>&& bytecode);

	ShaderBytecode GetBytecode() const noexcept { return {m_bytecode.data(), m_bytecode.size()}; }

	std::vector<std::uint8_t>&& TakeBytecode() noexcept { return std::move(m_bytecode); }

	const ShaderReflection& GetReflection() const noexcept { return m_reflection; }

	ShaderReflection&& TakeReflection() noexcept { return std::move(m_reflection); }

	void SetReflection(ShaderReflection&& reflection) noexcept { m_reflection = std::move(reflection); }

	const ShaderDebugArtifactSet& GetDebugArtifacts() const noexcept { return m_debugArtifacts; }

	ShaderDebugArtifactSet&& TakeDebugArtifacts() noexcept { return std::move(m_debugArtifacts); }

	void SetDebugArtifacts(ShaderDebugArtifactSet&& debugArtifacts) noexcept { m_debugArtifacts = std::move(debugArtifacts); }

private:
	std::vector<std::uint8_t> m_bytecode;
	ShaderReflection m_reflection;
	ShaderDebugArtifactSet m_debugArtifacts;
};
