#include "PCH.h"

#include "Verification/ShaderParameterStructVerifier.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Json/JsonWriter.h"

#include <algorithm>
#include <format>
#include <sstream>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_shaderParameterStructVerifierLogger, "ShaderCompiler.ParameterStructVerifier");

namespace ShaderParameterVerification
{
	static const char* GetResourceKindName(CookedShaderResourceKind kind) noexcept
	{
		switch (kind)
		{
			case CookedShaderResourceKind::Unknown:
				return "Unknown";
			case CookedShaderResourceKind::ConstantBuffer:
				return "ConstantBuffer";
			case CookedShaderResourceKind::Texture:
				return "Texture";
			case CookedShaderResourceKind::StructuredBuffer:
				return "StructuredBuffer";
			case CookedShaderResourceKind::ByteAddressBuffer:
				return "ByteAddressBuffer";
			case CookedShaderResourceKind::TypedBuffer:
				return "TypedBuffer";
			case CookedShaderResourceKind::RWTexture:
				return "RWTexture";
			case CookedShaderResourceKind::RWStructuredBuffer:
				return "RWStructuredBuffer";
			case CookedShaderResourceKind::RWByteAddressBuffer:
				return "RWByteAddressBuffer";
			case CookedShaderResourceKind::RWTypedBuffer:
				return "RWTypedBuffer";
			case CookedShaderResourceKind::Sampler:
				return "Sampler";
			case CookedShaderResourceKind::AccelerationStructure:
				return "AccelerationStructure";
			case CookedShaderResourceKind::PushConstantBlock:
				return "PushConstantBlock";
		}
		Diagnostics::Fatal(g_shaderParameterStructVerifierLogger, __FILE__, __LINE__, "Unknown cooked shader resource kind.");
	}

	static const char* GetResourceDimensionName(CookedShaderResourceDimension dimension) noexcept
	{
		switch (dimension)
		{
			case CookedShaderResourceDimension::Unknown:
				return "Unknown";
			case CookedShaderResourceDimension::Buffer:
				return "Buffer";
			case CookedShaderResourceDimension::Texture1D:
				return "Texture1D";
			case CookedShaderResourceDimension::Texture1DArray:
				return "Texture1DArray";
			case CookedShaderResourceDimension::Texture2D:
				return "Texture2D";
			case CookedShaderResourceDimension::Texture2DArray:
				return "Texture2DArray";
			case CookedShaderResourceDimension::Texture2DMS:
				return "Texture2DMS";
			case CookedShaderResourceDimension::Texture2DMSArray:
				return "Texture2DMSArray";
			case CookedShaderResourceDimension::Texture3D:
				return "Texture3D";
			case CookedShaderResourceDimension::TextureCube:
				return "TextureCube";
			case CookedShaderResourceDimension::TextureCubeArray:
				return "TextureCubeArray";
		}
		Diagnostics::Fatal(g_shaderParameterStructVerifierLogger, __FILE__, __LINE__, "Unknown cooked shader resource dimension.");
	}

	static const ShaderReflectionResourceBinding* FindReflectionBinding(const ShaderReflection& reflection, std::string_view name) noexcept
	{
		const auto binding = std::ranges::find(reflection.Bindings, name, &ShaderReflectionResourceBinding::Name);
		return binding != reflection.Bindings.end() ? &*binding : nullptr;
	}

	static const ShaderParameterStructFieldDescriptor* FindDescriptorField(const ShaderParameterStructDescriptor& descriptor, std::string_view name) noexcept
	{
		const auto field = std::ranges::find_if(descriptor.Fields, [name](const auto& candidate) { return candidate.Reflected && candidate.Name == name; });
		return field != descriptor.Fields.end() ? &*field : nullptr;
	}
}

std::string ShaderParameterStructVerificationResult::BuildJsonReport() const
{
	std::ostringstream stream;
	stream << "{\n";
	stream << "  \"status\": \"" << (mismatches.empty() ? "matched" : "mismatch") << "\",\n";
	stream << "  \"diagnostics\": [\n";
	for (std::size_t index = 0; index < diagnostics.size(); ++index)
	{
		stream << "    " << Json::QuoteString(diagnostics[index]);
		if (index + 1 < diagnostics.size())
		{
			stream << ',';
		}
		stream << "\n";
	}
	stream << "  ]\n";
	stream << "}\n";
	return stream.str();
}

namespace ShaderParameterVerification
{
	static void VerifyAutomaticValues(const ShaderParameterStructDescriptor& descriptor, const ShaderReflection& reflection, bool allowUnreflectedDeclarations, std::vector<std::string>& mismatches)
	{
		if (!descriptor.Values.empty())
		{
			const auto block = std::ranges::find_if(reflection.ConstantBuffers, [&descriptor](const auto& buffer) { return buffer.Name == descriptor.AutoParametersName; });
			if (block != reflection.ConstantBuffers.end())
			{
				for (const auto& value : descriptor.Values)
				{
					const auto member = std::ranges::find_if(block->Members, [&value](const auto& reflected) { return reflected.Name == value.Name; });
					const bool bReflectedMemberMismatchInBytes = member == block->Members.end() || member->OffsetInBytes != value.OffsetInBytes;
					const bool reflectedMemberMismatch = bReflectedMemberMismatchInBytes || (member->SizeInBytes != value.SizeInBytes);
					
					if (reflectedMemberMismatch)
					{
						mismatches.push_back(
						    std::format(
						        "SC2007 automatic parameter layout mismatch: block='{}' value='{}' expectedOffset={} expectedSize={}",
						        descriptor.AutoParametersName,
						        value.Name,
						        value.OffsetInBytes,
						        value.SizeInBytes));
					}
				}
			}
			else if (!allowUnreflectedDeclarations)
			{
				mismatches.push_back(std::format("SC2007 automatic parameter block '{}' has no reflected members.", descriptor.AutoParametersName));
			}
		}
	}

	static void VerifyDeclaredBindings(
	    const ShaderParameterStructDescriptor& descriptor,
	    const ShaderReflection& reflection,
	    bool allowUnreflectedDeclarations,
	    ShaderParameterStructVerificationResult& result)
	{
		for (const ShaderParameterStructFieldDescriptor& field : descriptor.Fields)
		{
			if (!field.Reflected)
			{
				continue;
			}

			const ShaderReflectionResourceBinding* binding = FindReflectionBinding(reflection, field.Name);
			if (binding == nullptr)
			{
				if (allowUnreflectedDeclarations)
				{
					result.diagnostics.push_back(
					    std::format(
					        "SC2006 declared binding is not referenced by this library export: name='{}' declaredKind='{}' "
					        "declaredDimension='{}'",
					        field.Name,
					        GetResourceKindName(field.Kind),
					        GetResourceDimensionName(field.Dimension)));

					continue;
				}

				result.mismatches.push_back(
				    std::format(
				        "SC2001 missing reflected binding: name='{}' declaredKind='{}' declaredDimension='{}'",
				        field.Name,
				        GetResourceKindName(field.Kind),
				        GetResourceDimensionName(field.Dimension)));

				continue;
			}

			if (binding->Kind != field.Kind)
			{
				result.mismatches.push_back(
				    std::format(
				        "SC2002 reflected kind mismatch: name='{}' reflectedBinding='{}' declaredKind='{}' reflectedKind='{}'",
				        field.Name,
				        binding->Name,
				        GetResourceKindName(field.Kind),
				        GetResourceKindName(binding->Kind)));
			}

			if (field.Dimension != CookedShaderResourceDimension::Unknown && binding->Dimension != CookedShaderResourceDimension::Unknown && binding->Dimension != field.Dimension)
			{
				result.mismatches.push_back(
				    std::format(
				        "SC2003 reflected dimension mismatch: name='{}' reflectedBinding='{}' declaredDimension='{}' reflectedDimension='{}'",
				        field.Name,
				        binding->Name,
				        GetResourceDimensionName(field.Dimension),
				        GetResourceDimensionName(binding->Dimension)));
			}

			if (binding->ArrayCount != field.ArrayCount)
			{
				result.mismatches.push_back(
				    std::format(
				        "SC2004 reflected array-count mismatch: name='{}' reflectedBinding='{}' declaredCount={} reflectedCount={}",
				        field.Name,
				        binding->Name,
				        field.ArrayCount,
				        binding->ArrayCount));
			}
		}
	}

	static void ReportUndeclaredBindings(const ShaderParameterStructDescriptor& descriptor, const ShaderReflection& reflection, std::vector<std::string>& diagnostics)
	{
		for (const ShaderReflectionResourceBinding& binding : reflection.Bindings)
		{
			if (FindDescriptorField(descriptor, binding.Name) == nullptr)
			{
				diagnostics.push_back(
				    std::format(
				        "SC2005 extra reflected binding: reflectedBinding='{}' reflectedKind='{}' reflectedDimension='{}' is not declared in "
				        "parameter struct '{}'",
				        binding.Name,
				        GetResourceKindName(binding.Kind),
				        GetResourceDimensionName(binding.Dimension),
				        descriptor.Name));
			}
		}
	}

	static void PublishDiagnostics(std::string_view descriptorName, ShaderParameterStructVerificationResult& result)
	{
		result.diagnostics.insert(result.diagnostics.begin(), result.mismatches.begin(), result.mismatches.end());
		if (result.mismatches.empty())
		{
			result.diagnostics.push_back(std::format("SC2000 parameter struct '{}' matches reflected resource bindings", descriptorName));
		}
	}

}

ShaderParameterStructVerificationResult VerifyShaderParameterStruct(const ShaderParameterStructDescriptor& descriptor, const ShaderReflection& reflection, bool allowUnreflectedDeclarations)
{
	ShaderParameterStructVerificationResult result;
	ShaderParameterVerification::VerifyAutomaticValues(descriptor, reflection, allowUnreflectedDeclarations, result.mismatches);
	ShaderParameterVerification::VerifyDeclaredBindings(descriptor, reflection, allowUnreflectedDeclarations, result);
	ShaderParameterVerification::ReportUndeclaredBindings(descriptor, reflection, result.diagnostics);
	ShaderParameterVerification::PublishDiagnostics(descriptor.Name, result);
	return result;
}
