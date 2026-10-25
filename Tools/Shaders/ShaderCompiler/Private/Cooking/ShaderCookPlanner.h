#pragma once

#include "Cooking/ShaderCookSettings.h"
#include "Cooking/ShaderCookTypes.h"
#include "ShaderContractCatalog.h"

#include <vector>

class ShaderDependencyManifest;

std::vector<ShaderCookDesc> SelectShadersForCook(const ShaderCookSettings& settings, const ShaderDependencyManifest& dependencyManifest, const ShaderContractCatalog& catalog);
