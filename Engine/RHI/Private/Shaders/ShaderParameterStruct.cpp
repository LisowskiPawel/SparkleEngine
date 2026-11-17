#include "PCH.h"

#include "Shaders/Authoring/ShaderParameterStruct.h"

#include "Core/Public/Hash/HashUtils.h"

ShaderParameterStructDescriptor ShaderParameterStructDescriptor::Build(
    std::string_view name,
    std::span<const ShaderParameterStructFieldDescriptor> fields,
    std::span<const ShaderParameterValueDescriptor> values)
{
	ShaderParameterStructDescriptor descriptor;
	descriptor.Name.assign(name);
	descriptor.Fields.assign(fields.begin(), fields.end());
	descriptor.Values.assign(values.begin(), values.end());
	if (!descriptor.Values.empty())
	{
		descriptor.AutoParametersName = "AutoParameters_" + std::string(name);
		std::uint32_t size = 0;
		std::uint64_t layoutHash = Hash::kFnv64OffsetBasis;
		for (auto& value : descriptor.Values)
		{
			if (value.SizeInBytes > 16u || size % 16u + value.SizeInBytes > 16u)
			{
				size = (size + 15u) & ~15u;
			}
			value.OffsetInBytes = size;
			size += value.SizeInBytes;
			layoutHash = Hash::ContinueFnv1a64(layoutHash, value.Name.data(), value.Name.size());
			layoutHash = Hash::ContinueFnv1a64(layoutHash, value.HlslType.data(), value.HlslType.size());
			layoutHash = Hash::ContinueFnv1a64Value(layoutHash, value.OffsetInBytes);
			layoutHash = Hash::ContinueFnv1a64Value(layoutHash, value.SizeInBytes);
		}

		descriptor.Fields.push_back(
		    ShaderParameterStructFieldDescriptor{
		        .Name = descriptor.AutoParametersName,
		        .Kind = CookedShaderResourceKind::ConstantBuffer,
		        .Dimension = CookedShaderResourceDimension::Buffer,
		        .SemanticKind = ShaderParameterSemanticKind::UniformData,
		        .ResourceDomain = ShaderParameterResourceDomain::Uniform,
		        .ValueSizeInBytes = (size + 15u) & ~15u,
		        .ValueAlignmentInBytes = 16u,
		        .ValueLayoutHash = Hash::FinalizeFnv1a64(layoutHash)});
	}
	return descriptor;
}
