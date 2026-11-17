#pragma once

#include "Verification/ShaderParameterStructVerifier.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct ShaderDebugArtifactSet final
{
	std::vector<std::string> CompileArguments;
	std::string CompilerOutput;
	std::string Disassembly;
	std::optional<ShaderParameterStructVerificationResult> ParameterVerification;
	std::string PreprocessedSource;
	std::vector<std::uint8_t> Symbols;
	std::string SymbolName;
	std::vector<std::uint8_t> CompilerLookupKey;
};
