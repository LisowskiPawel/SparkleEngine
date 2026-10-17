#pragma once

#include "Cooking/ShaderCookOutput.h"

#include <filesystem>

struct ShaderCookPipelinePlan;

namespace ShaderArtifactWriter
{
	ShaderCookOutput Write(
	    const ShaderCookPipelinePlan& plan,
	    bool replaceCompleteCatalog,
	    const std::filesystem::path& mapPath,
	    const std::filesystem::path& libraryPath,
	    const std::filesystem::path& stagedMapPath,
	    const std::filesystem::path& stagedLibraryPath);
}
