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
class ReflectionSerializer final
{
public:
	struct Output
	{
		std::vector<CookedShaderReflectionRecord> reflectionRecords;
		std::vector<CookedShaderResourceBindingRecord> resourceBindings;
		std::vector<CookedShaderConstantBufferRecord> constantBuffers;
		std::vector<CookedShaderConstantBufferMemberRecord> constantBufferMembers;
		std::vector<CookedShaderInputElementRecord> inputElements;
		std::vector<CookedShaderPushConstantRangeRecord> pushConstantRanges;
		std::vector<CookedShaderSpecializationConstantRecord> specializationConstants;
	};

	static ShaderReflection Decode(const GlobalShaderMap& map, const GlobalShaderMapEntry& entry);

	static void Build(std::span<const ShaderReflection> reflections, Strings::StringTableBuilder& stringTable, Output& outOutput);

private:
	static std::string ResolveString(const GlobalShaderMap& map, std::uint32_t offset, std::uint32_t size);
};
