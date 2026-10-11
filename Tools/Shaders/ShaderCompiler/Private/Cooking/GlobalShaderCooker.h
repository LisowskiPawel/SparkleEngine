#pragma once

#include "ToolWorkProgressEvent.h"
#include "Cooking/ShaderCookResult.h"
#include "Cooking/ShaderCookSettings.h"

#include <span>

struct ShaderCompileResult;
struct ShaderCookPipelinePlan;

class GlobalShaderCooker final
{
public:
	ShaderCookResult CookAll(const ShaderCookSettings& settings = {}, const ToolWorkProgressCallback& progress = {}) const;

private:
	static void PrepareShaderProducts(ShaderCookPipelinePlan& plan, std::span<ShaderCompileResult> compileResults);
};
