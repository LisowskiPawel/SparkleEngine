#pragma once

#include "Cooking/ShaderCookContext.h"
#include "ToolWorkProgressEvent.h"

class ShaderBackendPool;
struct ShaderCookSettings;

ShaderCookPipelinePlan BuildShaderCookPlan(const ShaderCookSettings& settings, ShaderBackendPool& backendPool, const ToolWorkProgressCallback& progress);
