#include "PCH.h"

#include "Passes/Presentation/PostProcessing/ChromaticAberrationShader.h"

IMPLEMENT_GLOBAL_SHADER(ChromaticAberrationCS, "/Engine/Passes/Presentation/PostProcessing/ChromaticAberration.hlsl", "main", Compute);
