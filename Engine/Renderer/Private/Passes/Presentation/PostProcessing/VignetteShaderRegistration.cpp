#include "PCH.h"

#include "Passes/Presentation/PostProcessing/VignetteShader.h"

IMPLEMENT_GLOBAL_SHADER(VignetteCS, "/Engine/Passes/Presentation/PostProcessing/Vignette.hlsl", "main", Compute);
