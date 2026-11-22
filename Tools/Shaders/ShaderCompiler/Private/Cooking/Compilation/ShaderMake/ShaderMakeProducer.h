#pragma once

#include <cstddef>
#include <filesystem>
#include <string>

struct ShaderCompileJob;

// Invocation-local association; paths are relative to its sealed source root.
struct ShaderMakeProducer final
{
	const ShaderCompileJob& Job;
	std::size_t ResultIndex = 0;
	std::string Name;
	std::filesystem::path Input;
	std::filesystem::path Code;
	std::filesystem::path Reflection;
	std::filesystem::path Preprocessed;
	std::filesystem::path Symbols;
};
