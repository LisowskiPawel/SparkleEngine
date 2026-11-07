#pragma once

#include "RHI/Public/Shaders/ShaderTarget.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct ShaderCookSettings final
{
	std::vector<ShaderTarget> targets = {ShaderTarget::DxilSm66, ShaderTarget::SpirV16};
	std::string backendName = "auto";
	std::string shaderId;
	std::vector<std::string> changedVirtualPaths;
	std::filesystem::path debugArtifactDirectory;
	std::vector<std::string> analysisPasses;
	bool bEnableDebugInfo = false;
	bool bEnableOptimizations = true;
	bool bTreatWarningsAsErrors = true;
	bool bStripDebugInfo = true;
	std::uint32_t maximumParallelCompiles = 4;
};
