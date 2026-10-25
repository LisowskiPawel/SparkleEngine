#pragma once

#include "ShaderContractCatalog.h"

#include <cstdint>
#include <string>
#include <string_view>

enum class ShaderContractSelectionKind : std::uint8_t
{
	All,
	ShaderId,
};

ShaderContractCatalog BuildShaderContractCatalog(ShaderContractSelectionKind selectionKind, std::string_view requestedId);
