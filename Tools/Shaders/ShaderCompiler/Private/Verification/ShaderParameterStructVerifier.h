#pragma once

struct ShaderCompileJob;
struct CookedStageBuild;

void VerifyCookedShaderParameters(const ShaderCompileJob& job, const CookedStageBuild& compiledStage);
