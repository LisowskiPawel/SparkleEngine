#pragma once

#include "Compiler/Output/ShaderReflection.h"

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#include <string_view>
#include <span>
#include <cstdint>
#include <ObjIdl.h>
#include <Unknwn.h>

#include <d3d12shader.h>
#include <dxcapi.h>

// Reads the compiler's reflection payload independently of how compilation ran.
ShaderReflection ExtractDxilReflection(IDxcUtils& utils, std::span<const std::uint8_t> reflectionBytes, ShaderStage stage);
ShaderReflection ExtractDxilLibraryReflection(IDxcUtils& utils, std::span<const std::uint8_t> reflectionBytes, std::string_view entryPoint);
