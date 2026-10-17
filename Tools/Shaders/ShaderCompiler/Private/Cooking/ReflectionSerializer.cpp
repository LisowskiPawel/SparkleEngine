#include "PCH.h"

#include "Cooking/ReflectionSerializer.h"

#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include <utility>

std::string ReflectionSerializer::ResolveString(const GlobalShaderMap& map, std::uint32_t offset, std::uint32_t size)
{
	return std::string(map.ResolveString(ShaderMapStringRef{offset, size}));
}

void ReflectionSerializer::Build(std::span<const ShaderReflection> reflections, Strings::StringTableBuilder& stringTable, Output& outOutput)
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
		// Emit constant buffers before bindings so CB indices are already stable.
		const std::uint32_t cbBaseIndex = static_cast<std::uint32_t>(outOutput.constantBuffers.size());
		for (const ShaderReflectionConstantBuffer& cb : reflection.ConstantBuffers)
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

		const std::uint32_t bindingOffset = static_cast<std::uint32_t>(outOutput.resourceBindings.size());
		for (const ShaderReflectionResourceBinding& binding : reflection.Bindings)
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

		const std::uint32_t inputOffset = static_cast<std::uint32_t>(outOutput.inputElements.size());
		for (const ShaderReflectionInputElement& element : reflection.InputElements)
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

		const std::uint32_t pushOffset = static_cast<std::uint32_t>(outOutput.pushConstantRanges.size());
		for (const ShaderReflectionPushConstantRange& range : reflection.PushConstants)
		{
			outOutput.pushConstantRanges.push_back(CookedShaderPushConstantRangeRecord{.OffsetInBytes = range.OffsetInBytes, .SizeInBytes = range.SizeInBytes, .VisibilityMask = range.VisibilityMask});
		}

		const std::uint32_t specOffset = static_cast<std::uint32_t>(outOutput.specializationConstants.size());
		for (const ShaderReflectionSpecializationConstant& spec : reflection.SpecializationConstants)
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

		CookedShaderReflectionRecord record{};
		record.ResourceBindingOffset = bindingOffset;
		record.ResourceBindingCount = static_cast<std::uint32_t>(reflection.Bindings.size());
		record.ConstantBufferOffset = cbBaseIndex;
		record.ConstantBufferCount = static_cast<std::uint32_t>(reflection.ConstantBuffers.size());
		record.InputElementOffset = inputOffset;
		record.InputElementCount = static_cast<std::uint32_t>(reflection.InputElements.size());
		record.PushConstantRangeOffset = pushOffset;
		record.PushConstantRangeCount = static_cast<std::uint32_t>(reflection.PushConstants.size());
		record.SpecializationConstantOffset = specOffset;
		record.SpecializationConstantCount = static_cast<std::uint32_t>(reflection.SpecializationConstants.size());
		record.ThreadGroupSize[0] = reflection.ThreadGroupSize[0];
		record.ThreadGroupSize[1] = reflection.ThreadGroupSize[1];
		record.ThreadGroupSize[2] = reflection.ThreadGroupSize[2];
		record.EntryFlags = reflection.EntryFlags;
		record.WaveSize = reflection.WaveSize;
		outOutput.reflectionRecords.push_back(record);
	}
}

ShaderReflection ReflectionSerializer::Decode(const GlobalShaderMap& map, const GlobalShaderMapEntry& entry)
{
	ShaderReflection result;
	const CookedShaderReflectionRecord& record = map.GetReflection(entry);
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
	const auto allPushConstants = map.GetPushConstantRanges();
	for (std::uint32_t index = 0; index < record.PushConstantRangeCount; ++index)
	{
		const CookedShaderPushConstantRangeRecord& range = allPushConstants[record.PushConstantRangeOffset + index];
		result.PushConstants.push_back(ShaderReflectionPushConstantRange{.OffsetInBytes = range.OffsetInBytes, .SizeInBytes = range.SizeInBytes, .VisibilityMask = range.VisibilityMask});
	}
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
	result.ThreadGroupSize = {record.ThreadGroupSize[0], record.ThreadGroupSize[1], record.ThreadGroupSize[2]};
	result.EntryFlags = record.EntryFlags;
	result.WaveSize = record.WaveSize;
	return result;
}
