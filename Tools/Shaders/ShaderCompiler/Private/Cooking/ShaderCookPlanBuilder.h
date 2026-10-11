#pragma once

#include "Cooking/ShaderCookContext.h"
#include "ToolWorkProgressEvent.h"

class ShaderBackendPool;
struct ShaderCookSettings;

namespace ShaderCookPlanBuilder
{
	ShaderCookPipelinePlan Build(const ShaderCookSettings& settings, ShaderBackendPool& backendPool, const ToolWorkProgressCallback& progress);
}
