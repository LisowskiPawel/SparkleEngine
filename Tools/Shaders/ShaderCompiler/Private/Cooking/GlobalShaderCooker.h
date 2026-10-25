#pragma once

#include "ToolWorkProgressEvent.h"
#include "Cooking/ShaderCookResult.h"
#include "Cooking/ShaderCookSettings.h"

ShaderCookResult CookGlobalShaders(const ShaderCookSettings& settings = {}, const ToolWorkProgressCallback& progress = {});
