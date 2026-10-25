#pragma once

#include "ShaderContractCatalog.h"

#include <string>
#include <vector>

std::vector<ShaderContractVerificationFailure> ValidateShaderContracts(const ShaderContractCatalog& catalog);
std::string FormatShaderContractFailure(const ShaderContractVerificationFailure& failure);
