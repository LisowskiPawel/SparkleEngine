#pragma once

#include "Cooking/CookedStageBuild.h"
#include "Cooking/ShaderCompileJob.h"

#include <string>
#include <string_view>

struct ShaderDebugArtifactSet;

void VerifyCookedShaderParameters(const ShaderCompileJob& job, const CookedStageBuild& compiledStage, ShaderDebugArtifactSet* debugArtifacts);
