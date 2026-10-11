#pragma once

#include "Compiler/ShaderCompileRequest.h"
#include "Cooking/CookedStageBuild.h"
#include "ShaderDebugArtifactSet.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

using ShaderCompileInputHash = std::uint64_t;

struct ShaderCompileJob final
{
	ShaderCompileRequest Request;
	std::string BackendName;
	std::string TargetProfile;
	std::uint64_t BackendVersion = 0;
	std::uint64_t SourceContentHash = 0;
	std::uint64_t DependencyClosureHash = 0;
	std::uint64_t RequestHash = 0;
	ShaderCompileInputHash InputHash = 0;
	std::vector<std::string> VirtualDependencies;

	// Publication destination in the selected catalog, excluded from compiler-input identity.
	std::size_t ShaderIndex = 0;
};

struct ShaderCompileResult final
{
	ShaderTypeId ShaderType = 0;
	ShaderTarget Target = kDefaultShaderTarget;
	ShaderCompileInputHash InputHash = 0;
	CookedStageBuild Output;
	ShaderDebugArtifactSet DebugArtifacts;
};
