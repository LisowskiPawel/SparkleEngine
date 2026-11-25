#pragma once

#include "Diagnostics/AssetCookerDiagnostics.h"
#include "Planning/ProjectCookPlan.h"

#include <filesystem>
#include <span>

bool BuildTextureCookRequests(std::span<const AssetCookerSceneEntry> sceneEntries, AssetCookerDiagnostics& diagnostics, const std::filesystem::path& outputPath);
