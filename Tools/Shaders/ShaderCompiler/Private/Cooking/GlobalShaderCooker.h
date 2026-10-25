#pragma once

#include "ToolWorkProgressEvent.h"
#include "Cooking/ShaderCookResult.h"
#include "Cooking/ShaderCookSettings.h"

#include <span>

struct ShaderCompileResult;
struct ShaderCookPipelinePlan;

ShaderCookResult CookGlobalShaders(const ShaderCookSettings& settings = {}, const ToolWorkProgressCallback& progress = {});
