#pragma once

#include "Diagnostics/AssetCookerDiagnostics.h"
#include "Planning/ProjectCookPlan.h"

bool ValidateShaderCookStage(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics);
bool ValidateTextureCookStage(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics);
bool CookShaders(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outputs);
bool CookTextures(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outputs);
bool CookSceneAssets(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outputs);
