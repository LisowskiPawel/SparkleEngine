#pragma once

#include "../Diagnostics/AssetCookerDiagnostics.h"
#include "../Planning/ProjectCookPlan.h"

#include <cstddef>

bool DispatchAssetCookPlan(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::size_t& productCount);
