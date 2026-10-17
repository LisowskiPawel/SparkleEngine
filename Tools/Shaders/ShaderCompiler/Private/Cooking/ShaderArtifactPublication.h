#pragma once

#include "Cooking/ShaderCookContext.h"
#include "Cooking/ShaderCookOutput.h"

#include <filesystem>
#include <span>

struct ShaderCookSettings;

class ShaderArtifactPublication final
{
public:
	ShaderArtifactPublication() = delete;

	static ShaderCookOutput Publish(
	    const ShaderCookPipelinePlan& plan,
	    std::span<const ShaderCompileResult> compileResults,
	    const ShaderCookSettings& settings,
	    const std::filesystem::path& outputDirectory,
	    bool replaceCompleteCatalog);
};
