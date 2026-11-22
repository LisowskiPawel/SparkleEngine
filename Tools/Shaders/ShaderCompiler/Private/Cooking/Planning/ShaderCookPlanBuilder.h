#pragma once

#include "Cooking/Planning/ShaderCookPipelinePlan.h"
#include "ToolWorkProgressEvent.h"

struct ShaderCookSettings;

ShaderCookPipelinePlan BuildShaderCookPlan(const ShaderCookSettings& settings, const ToolWorkProgressCallback& progress);
