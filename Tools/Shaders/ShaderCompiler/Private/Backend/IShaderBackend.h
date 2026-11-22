#pragma once

#include "Compiler/ShaderCompileRequest.h"
#include "Compiler/Output/CompiledShader.h"

#include <cstdint>
#include <string_view>

// Compile execution owns backend instances; planning consumes backend descriptors.
class IShaderBackend
{
public:
	virtual ~IShaderBackend();

	IShaderBackend(const IShaderBackend&) = delete;
	IShaderBackend& operator=(const IShaderBackend&) = delete;
	IShaderBackend(IShaderBackend&&) = delete;
	IShaderBackend& operator=(IShaderBackend&&) = delete;

	// Stable backend identity for cooked binaries and compile-input hashes.
	// Different backends must never collide on the same artifact slot.
	virtual std::string_view GetBackendName() const = 0;
	virtual std::uint64_t GetBackendVersion() const = 0;

	virtual CompiledShader Compile(const ShaderCompileRequest& request) = 0;

protected:
	IShaderBackend();
};
