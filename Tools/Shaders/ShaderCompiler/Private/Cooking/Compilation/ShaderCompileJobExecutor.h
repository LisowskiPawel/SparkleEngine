#pragma once

struct ShaderCompileJob;
struct ShaderCompileResult;

namespace ShaderCompileJobExecutor
{
	ShaderCompileResult Execute(const ShaderCompileJob& job);
}
