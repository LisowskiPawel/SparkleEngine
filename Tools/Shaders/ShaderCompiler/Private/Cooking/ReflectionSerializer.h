#pragma once

#include "Core/Public/Strings/StringTableBuilder.h"
#include "RHI/Public/Shaders/ShaderReflection.h"
#include "ShaderReflection.h"

#include <cstdint>
#include <string>
#include <span>
#include <vector>

class GlobalShaderMap;
struct GlobalShaderMapEntry;

// Encodes and decodes per-stage reflection in global shader map POD arrays.
// Strings are interned through the caller-owned StringTableBuilder.
struct ShaderReflectionTables
{
	std::vector<CookedShaderReflectionRecord> reflectionRecords;
	std::vector<CookedShaderResourceBindingRecord> resourceBindings;
	std::vector<CookedShaderConstantBufferRecord> constantBuffers;
	std::vector<CookedShaderConstantBufferMemberRecord> constantBufferMembers;
	std::vector<CookedShaderInputElementRecord> inputElements;
	std::vector<CookedShaderPushConstantRangeRecord> pushConstantRanges;
	std::vector<CookedShaderSpecializationConstantRecord> specializationConstants;
};

ShaderReflection DecodeShaderReflection(const GlobalShaderMap& map, const GlobalShaderMapEntry& entry);

void BuildShaderReflectionTables(std::span<const ShaderReflection> reflections, Strings::StringTableBuilder& stringTable, ShaderReflectionTables& outOutput);
