#pragma once

#include "Diagnostics/AssetCookerDiagnostics.h"
#include "CookedSceneBuild.h"

#include <filesystem>

CookedSceneBuild CookImportedScene(const std::filesystem::path& sourcePath, AssetCookerDiagnostics& diagnostics);
