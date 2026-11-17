#pragma once

#include "Cooking/CookedStageBuild.h"
#include "Cooking/Compilation/ShaderCompileJob.h"

struct ShaderDebugArtifactSet;

void VerifyCookedShaderParameters(const ShaderCompileJob& job, const CookedStageBuild& compiledStage, ShaderDebugArtifactSet* debugArtifacts);
