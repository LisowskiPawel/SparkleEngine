#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

struct ShaderCompileInvocation final
{
	std::vector<std::string> Arguments;
	std::filesystem::path ConfigurationPath;
	std::string Configuration;
};

struct ShaderDebugArtifactSet final
{
	std::shared_ptr<const ShaderCompileInvocation> Invocation;
	std::shared_ptr<const ShaderCompileInvocation> PreprocessingInvocation;
	std::filesystem::path CompilerInputPath;
	std::string CompilerOutput;
	std::string Disassembly;
	std::string PreprocessedSource;
	std::vector<std::uint8_t> Symbols;
	std::string SymbolName;
};
