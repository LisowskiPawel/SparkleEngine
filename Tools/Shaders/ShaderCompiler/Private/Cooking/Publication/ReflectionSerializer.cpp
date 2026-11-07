#include "PCH.h"

#include "Cooking/Publication/ReflectionSerializer.h"

#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include <utility>

namespace ShaderReflectionSerialization
{
	static std::string ResolveString(const GlobalShaderMap& map, std::uint32_t offset, std::uint32_t size)
	{
		return std::string(map.ResolveString(ShaderMapStringRef{offset, size}));
	}

	static CookedShaderReflectionRecord BeginReflectionRecord(const ShaderReflection& reflection, const ShaderReflectionTables& outOutput) noexcept
	{
		CookedShaderReflectionRecord record{};
		record.ResourceBindingOffset = static_cast<std::uint32_t>(outOutput.resourceBindings.size());
		record.ResourceBindingCount = static_cast<std::uint32_t>(reflection.Bindings.size());
		record.ConstantBufferOffset = static_cast<std::uint32_t>(outOutput.constantBuffers.size());
		record.ConstantBufferCount = static_cast<std::uint32_t>(reflection.ConstantBuffers.size());
		record.InputElementOffset = static_cast<std::uint32_t>(outOutput.inputElements.size());
		record.InputElementCount = static_cast<std::uint32_t>(reflection.InputElements.size());
		record.PushConstantRangeOffset = static_cast<std::uint32_t>(outOutput.pushConstantRanges.size());
		record.PushConstantRangeCount = static_cast<std::uint32_t>(reflection.PushConstants.size());
		record.SpecializationConstantOffset = static_cast<std::uint32_t>(outOutput.specializationConstants.size());
		record.SpecializationConstantCount = static_cast<std::uint32_t>(reflection.SpecializationConstants.size());
		record.ThreadGroupSize[0] = reflection.ThreadGroupSize[0];
		record.ThreadGroupSize[1] = reflection.ThreadGroupSize[1];
		record.ThreadGroupSize[2] = reflection.ThreadGroupSize[2];
		record.EntryFlags = reflection.EntryFlags;
		record.WaveSize = reflection.WaveSize;
		return record;
	}

	static void AppendConstantBuffers(std::span<const ShaderReflectionConstantBuffer> constantBuffers, Strings::StringTableBuilder& stringTable, ShaderReflectionTables& outOutput)
	{
		for (const ShaderReflectionConstantBuffer& cb : constantBuffers)
		{
			const auto nameEntry = stringTable.Add(cb.Name);
			const std::uint32_t memberOffset = static_cast<std::uint32_t>(outOutput.constantBufferMembers.size());

			for (const ShaderReflectionConstantBufferMember& member : cb.Members)
			{
				const auto memberNameEntry = stringTable.Add(member.Name);

				outOutput.constantBufferMembers.push_back(
				    CookedShaderConstantBufferMemberRecord{
				        .NameOffsetInBytes = memberNameEntry.OffsetInBytes,
				        .NameSizeInBytes = memberNameEntry.SizeInBytes,
				        .OffsetInBytes = member.OffsetInBytes,
				        .SizeInBytes = member.SizeInBytes,
				        .ArrayCount = member.ArrayCount,
				        .ArrayStrideInBytes = member.ArrayStrideInBytes,
				        .ScalarType = member.ScalarType,
				        .RowCount = member.RowCount,
				        .ColumnCount = member.ColumnCount});
			}

			outOutput.constantBuffers.push_back(
			    CookedShaderConstantBufferRecord{
			        .NameOffsetInBytes = nameEntry.OffsetInBytes,
			        .NameSizeInBytes = nameEntry.SizeInBytes,
			        .MemberOffset = memberOffset,
			        .MemberCount = static_cast<std::uint32_t>(cb.Members.size()),
			        .SizeInBytes = cb.SizeInBytes});
		}
	}

	static void AppendResourceBindings(
	    std::span<const ShaderReflectionResourceBinding> bindings,
	    Strings::StringTableBuilder& stringTable,
	    ShaderReflectionTables& outOutput,
	    std::uint32_t cbBaseIndex)
	{
		for (const ShaderReflectionResourceBinding& binding : bindings)
		{
			const auto nameEntry = stringTable.Add(binding.Name);
			std::uint32_t cbIndex = kCookedShaderReflectionInvalidIndex;
			if (binding.ConstantBufferIndex != kCookedShaderReflectionInvalidIndex)
			{
				cbIndex = cbBaseIndex + binding.ConstantBufferIndex;
			}

			outOutput.resourceBindings.push_back(
			    CookedShaderResourceBindingRecord{
			        .NameOffsetInBytes = nameEntry.OffsetInBytes,
			        .NameSizeInBytes = nameEntry.SizeInBytes,
			        .Kind = binding.Kind,
			        .Dimension = binding.Dimension,
			        .IsReadOnly = binding.IsReadOnly ? std::uint8_t{1} : std::uint8_t{0},
			        .Set = binding.Set,
			        .Slot = binding.Slot,
			        .ArrayCount = binding.ArrayCount,
			        .SizeInBytes = binding.SizeInBytes,
			        .ConstantBufferIndex = cbIndex});
		}
	}

	static void AppendInputElements(std::span<const ShaderReflectionInputElement> inputs, Strings::StringTableBuilder& stringTable, ShaderReflectionTables& outOutput)
	{
		for (const ShaderReflectionInputElement& element : inputs)
		{
			const auto semanticEntry = stringTable.Add(element.Semantic);

			outOutput.inputElements.push_back(
			    CookedShaderInputElementRecord{
			        .SemanticOffsetInBytes = semanticEntry.OffsetInBytes,
			        .SemanticSizeInBytes = semanticEntry.SizeInBytes,
			        .SemanticIndex = element.SemanticIndex,
			        .Location = element.Location,
			        .ScalarType = element.ScalarType,
			        .ComponentCount = element.ComponentCount});
		}
	}

	static void AppendPushConstants(std::span<const ShaderReflectionPushConstantRange> ranges, ShaderReflectionTables& outOutput)
	{
		for (const ShaderReflectionPushConstantRange& range : ranges)
		{
			outOutput.pushConstantRanges.push_back(CookedShaderPushConstantRangeRecord{.OffsetInBytes = range.OffsetInBytes, .SizeInBytes = range.SizeInBytes, .VisibilityMask = range.VisibilityMask});
		}
	}

	static void AppendSpecializationConstants(std::span<const ShaderReflectionSpecializationConstant> constants, Strings::StringTableBuilder& stringTable, ShaderReflectionTables& outOutput)
	{
		for (const ShaderReflectionSpecializationConstant& spec : constants)
		{
			const auto nameEntry = stringTable.Add(spec.Name);

			outOutput.specializationConstants.push_back(
			    CookedShaderSpecializationConstantRecord{
			        .NameOffsetInBytes = nameEntry.OffsetInBytes,
			        .NameSizeInBytes = nameEntry.SizeInBytes,
			        .ConstantId = spec.ConstantId,
			        .DefaultValueBits = spec.DefaultValueBits,
			        .ScalarType = spec.ScalarType});
		}
	}

	static void DecodeConstantBuffers(const GlobalShaderMap& map, const CookedShaderReflectionRecord& record, ShaderReflection& result)
	{
		const auto allConstantBuffers = map.GetConstantBuffers();
		const auto allMembers = map.GetConstantBufferMembers();
		for (std::uint32_t index = 0; index < record.ConstantBufferCount; ++index)
		{
			const CookedShaderConstantBufferRecord& buffer = allConstantBuffers[record.ConstantBufferOffset + index];
			ShaderReflectionConstantBuffer decoded;
			decoded.Name = ResolveString(map, buffer.NameOffsetInBytes, buffer.NameSizeInBytes);
			decoded.SizeInBytes = buffer.SizeInBytes;
			for (std::uint32_t memberIndex = 0; memberIndex < buffer.MemberCount; ++memberIndex)
			{
				const CookedShaderConstantBufferMemberRecord& member = allMembers[buffer.MemberOffset + memberIndex];

				decoded.Members.push_back(
				    ShaderReflectionConstantBufferMember{
				        .Name = ResolveString(map, member.NameOffsetInBytes, member.NameSizeInBytes),
				        .OffsetInBytes = member.OffsetInBytes,
				        .SizeInBytes = member.SizeInBytes,
				        .ArrayCount = member.ArrayCount,
				        .ArrayStrideInBytes = member.ArrayStrideInBytes,
				        .ScalarType = member.ScalarType,
				        .RowCount = member.RowCount,
				        .ColumnCount = member.ColumnCount});
			}
			result.ConstantBuffers.push_back(std::move(decoded));
		}
	}

	static void DecodeResourceBindings(const GlobalShaderMap& map, const CookedShaderReflectionRecord& record, ShaderReflection& result)
	{
		const auto allBindings = map.GetResourceBindings();
		for (std::uint32_t index = 0; index < record.ResourceBindingCount; ++index)
		{
			const CookedShaderResourceBindingRecord& binding = allBindings[record.ResourceBindingOffset + index];
			std::uint32_t constantBufferIndex = kCookedShaderReflectionInvalidIndex;
			if (binding.ConstantBufferIndex >= record.ConstantBufferOffset && binding.ConstantBufferIndex < record.ConstantBufferOffset + record.ConstantBufferCount)
			{
				constantBufferIndex = binding.ConstantBufferIndex - record.ConstantBufferOffset;
			}

			result.Bindings.push_back(
			    ShaderReflectionResourceBinding{
			        .Name = ResolveString(map, binding.NameOffsetInBytes, binding.NameSizeInBytes),
			        .Kind = binding.Kind,
			        .Dimension = binding.Dimension,
			        .IsReadOnly = binding.IsReadOnly != 0,
			        .Set = binding.Set,
			        .Slot = binding.Slot,
			        .ArrayCount = binding.ArrayCount,
			        .SizeInBytes = binding.SizeInBytes,
			        .ConstantBufferIndex = constantBufferIndex});
		}
	}

	static void DecodeInputElements(const GlobalShaderMap& map, const CookedShaderReflectionRecord& record, ShaderReflection& result)
	{
		const auto allInputs = map.GetInputElements();
		for (std::uint32_t index = 0; index < record.InputElementCount; ++index)
		{
			const CookedShaderInputElementRecord& input = allInputs[record.InputElementOffset + index];

			result.InputElements.push_back(
			    ShaderReflectionInputElement{
			        .Semantic = ResolveString(map, input.SemanticOffsetInBytes, input.SemanticSizeInBytes),
			        .SemanticIndex = input.SemanticIndex,
			        .Location = input.Location,
			        .ScalarType = input.ScalarType,
			        .ComponentCount = input.ComponentCount});
		}
	}

	static void DecodePushConstants(const GlobalShaderMap& map, const CookedShaderReflectionRecord& record, ShaderReflection& result)
	{
		const auto allPushConstants = map.GetPushConstantRanges();
		for (std::uint32_t index = 0; index < record.PushConstantRangeCount; ++index)
		{
			const CookedShaderPushConstantRangeRecord& range = allPushConstants[record.PushConstantRangeOffset + index];
			result.PushConstants.push_back(ShaderReflectionPushConstantRange{.OffsetInBytes = range.OffsetInBytes, .SizeInBytes = range.SizeInBytes, .VisibilityMask = range.VisibilityMask});
		}
	}

	static void DecodeSpecializationConstants(const GlobalShaderMap& map, const CookedShaderReflectionRecord& record, ShaderReflection& result)
	{
		const auto allSpecializations = map.GetSpecializationConstants();
		for (std::uint32_t index = 0; index < record.SpecializationConstantCount; ++index)
		{
			const CookedShaderSpecializationConstantRecord& value = allSpecializations[record.SpecializationConstantOffset + index];

			result.SpecializationConstants.push_back(
			    ShaderReflectionSpecializationConstant{
			        .Name = ResolveString(map, value.NameOffsetInBytes, value.NameSizeInBytes),
			        .ConstantId = value.ConstantId,
			        .DefaultValueBits = value.DefaultValueBits,
			        .ScalarType = value.ScalarType});
		}
	}
}

void BuildShaderReflectionTables(std::span<const ShaderReflection> reflections, Strings::StringTableBuilder& stringTable, ShaderReflectionTables& outOutput)
{
	outOutput.reflectionRecords.clear();
	outOutput.resourceBindings.clear();
	outOutput.constantBuffers.clear();
	outOutput.constantBufferMembers.clear();
	outOutput.inputElements.clear();
	outOutput.pushConstantRanges.clear();
	outOutput.specializationConstants.clear();

	outOutput.reflectionRecords.reserve(reflections.size());

	for (const ShaderReflection& reflection : reflections)
	{
		const auto record = ShaderReflectionSerialization::BeginReflectionRecord(reflection, outOutput);
		ShaderReflectionSerialization::AppendConstantBuffers(reflection.ConstantBuffers, stringTable, outOutput);
		ShaderReflectionSerialization::AppendResourceBindings(reflection.Bindings, stringTable, outOutput, record.ConstantBufferOffset);
		ShaderReflectionSerialization::AppendInputElements(reflection.InputElements, stringTable, outOutput);
		ShaderReflectionSerialization::AppendPushConstants(reflection.PushConstants, outOutput);
		ShaderReflectionSerialization::AppendSpecializationConstants(reflection.SpecializationConstants, stringTable, outOutput);
		outOutput.reflectionRecords.push_back(record);
	}
}

ShaderReflection DecodeShaderReflection(const GlobalShaderMap& map, const GlobalShaderMapEntry& entry)
{
	ShaderReflection result;
	const auto& record = map.GetReflection(entry);
	ShaderReflectionSerialization::DecodeConstantBuffers(map, record, result);
	ShaderReflectionSerialization::DecodeResourceBindings(map, record, result);
	ShaderReflectionSerialization::DecodeInputElements(map, record, result);
	ShaderReflectionSerialization::DecodePushConstants(map, record, result);
	ShaderReflectionSerialization::DecodeSpecializationConstants(map, record, result);
	result.ThreadGroupSize = {record.ThreadGroupSize[0], record.ThreadGroupSize[1], record.ThreadGroupSize[2]};
	result.EntryFlags = record.EntryFlags;
	result.WaveSize = record.WaveSize;
	return result;
}
