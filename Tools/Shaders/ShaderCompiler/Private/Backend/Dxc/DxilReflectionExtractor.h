#pragma once

#include "ShaderReflection.h"

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#include <string_view>
#include <ObjIdl.h>
#include <Unknwn.h>

#include <cstdint>
#include <d3d12shader.h>
#include <dxcapi.h>
#include <span>
#include <string>

// DXIL -> ShaderReflection extractor for compiled DXC binaries.
// DXC and D3D12 reflection symbols stay confined to Backends/Dxc/.
ShaderReflection ExtractDxilReflection(IDxcUtils& utils, IDxcResult* result, ShaderStage stage);
ShaderReflection ExtractDxilLibraryReflection(IDxcUtils& utils, IDxcResult* result, std::string_view entryPoint);

CookedShaderResourceKind MapDxilResourceKind(D3D_SHADER_INPUT_TYPE type, D3D_SRV_DIMENSION dim);
CookedShaderResourceDimension MapDxilDimension(D3D_SRV_DIMENSION dim);
bool IsDxilReadOnlyKind(CookedShaderResourceKind kind);
CookedShaderScalarType MapDxilScalarType(D3D_SHADER_VARIABLE_TYPE type);
