#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

enum class ShaderMakeOperation : std::uint8_t
{
	Compile,
	Preprocess,
};

struct ShaderMakeProducer;
struct ShaderCompileJob;

std::string BuildShaderMakeConfiguration(std::span<const ShaderMakeProducer> producers, ShaderMakeOperation operation);
std::vector<std::string> BuildShaderMakeArguments(const ShaderCompileJob& job, std::string configurationPath, std::string compilerPath, std::uint32_t workers, ShaderMakeOperation operation);
