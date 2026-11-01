#pragma once

#include "../Diagnostics/AssetCookerDiagnostics.h"
#include "../Planning/ProjectCookPlan.h"

#include <vector>
#include <filesystem>

bool DispatchAssetCookPlan(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outOutputs);
