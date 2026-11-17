#pragma once

#include <filesystem>

namespace ShaderCompilerPaths
{
	std::filesystem::path CanonicalizeForCompiler(const std::filesystem::path& path);
}
