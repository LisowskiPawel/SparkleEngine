#pragma once

#include "Cooking/Planning/Dependencies/ShaderDependencyManifest.h"
#include "Cooking/Compilation/ShaderCompileJob.h"
#include "Cooking/ShaderCookTypes.h"

#include <cstddef>
#include <vector>

struct ShaderCookPipelinePlan final
{
	std::vector<ShaderCookDesc> shaders;
	std::vector<ShaderCookOutputSet> shaderOutputs;
	std::vector<ShaderCompileJob> jobs;
	std::vector<ShaderTypeId> registeredShaderTypes;
	ShaderDependencyManifest dependencyManifest;
	std::size_t removedShaderTypeCount = 0;
};
