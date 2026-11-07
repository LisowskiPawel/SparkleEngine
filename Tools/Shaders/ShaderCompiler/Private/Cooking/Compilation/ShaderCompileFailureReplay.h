#pragma once

#include <filesystem>
#include <string_view>

struct ShaderCompileJob;

namespace ShaderCompileFailureReplay
{
	void Write(const std::filesystem::path& cookedShaderRoot, const ShaderCompileJob& job, std::string_view diagnostic) noexcept;
}
