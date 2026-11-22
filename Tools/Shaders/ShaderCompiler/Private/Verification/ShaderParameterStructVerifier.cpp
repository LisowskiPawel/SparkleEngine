#include "PCH.h"

#include "Verification/ShaderParameterStructVerifier.h"

#include "Cooking/ShaderCookDiagnostics.h"
#include "Cooking/Compilation/ShaderCompileJob.h"
#include "Cooking/CookedStageBuild.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <format>
#include <optional>
#include <string>

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
		return "Unknown";
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
		return "Unknown";
	}

	static const ShaderReflectionResourceBinding* FindReflectionBinding(const ShaderReflection& reflection, std::string_view name) noexcept
	{
		const auto binding = std::ranges::find(reflection.Bindings, name, &ShaderReflectionResourceBinding::Name);
		return binding != reflection.Bindings.end() ? &*binding : nullptr;
	}

	static std::optional<std::string> FindAutomaticValueMismatch(const ShaderParameterStructDescriptor& descriptor, const ShaderReflection& reflection, bool bAllowUnreflectedDeclarations)
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
					const bool bReflectedMemberMismatch = bReflectedMemberMismatchInBytes || (member->SizeInBytes != value.SizeInBytes);

					if (bReflectedMemberMismatch)
					{
						return std::format(
						    "SC2007 automatic parameter layout mismatch: block='{}' value='{}' expectedOffset={} expectedSize={}",
						    descriptor.AutoParametersName,
						    value.Name,
						    value.OffsetInBytes,
						    value.SizeInBytes);
					}
				}
			}
			else if (!bAllowUnreflectedDeclarations)
			{
				return std::format("SC2007 automatic parameter block '{}' has no reflected members.", descriptor.AutoParametersName);
			}
		}
		return std::nullopt;
	}

	static std::optional<std::string> FindDeclaredBindingMismatch(const ShaderParameterStructDescriptor& descriptor, const ShaderReflection& reflection, bool bAllowUnreflectedDeclarations)
	{
		for (const ShaderParameterStructFieldDescriptor& field : descriptor.Fields)
		{
			if (!field.bReflected)
			{
				continue;
			}

			const ShaderReflectionResourceBinding* binding = FindReflectionBinding(reflection, field.Name);
			if (binding == nullptr)
			{
				if (bAllowUnreflectedDeclarations)
				{
					continue;
				}

				return std::format(
				    "SC2001 missing reflected binding: name='{}' declaredKind='{}' declaredDimension='{}'",
				    field.Name,
				    GetResourceKindName(field.Kind),
				    GetResourceDimensionName(field.Dimension));
			}

			if (binding->Kind != field.Kind)
			{
				return std::format(
				    "SC2002 reflected kind mismatch: name='{}' reflectedBinding='{}' declaredKind='{}' reflectedKind='{}'",
				    field.Name,
				    binding->Name,
				    GetResourceKindName(field.Kind),
				    GetResourceKindName(binding->Kind));
			}

			if (field.Dimension != CookedShaderResourceDimension::Unknown && binding->Dimension != CookedShaderResourceDimension::Unknown && binding->Dimension != field.Dimension)
			{
				return std::format(
				    "SC2003 reflected dimension mismatch: name='{}' reflectedBinding='{}' declaredDimension='{}' reflectedDimension='{}'",
				    field.Name,
				    binding->Name,
				    GetResourceDimensionName(field.Dimension),
				    GetResourceDimensionName(binding->Dimension));
			}

			if (binding->ArrayCount != field.ArrayCount)
			{
				return std::format(
				    "SC2004 reflected array-count mismatch: name='{}' reflectedBinding='{}' declaredCount={} reflectedCount={}",
				    field.Name,
				    binding->Name,
				    field.ArrayCount,
				    binding->ArrayCount);
			}
		}
		return std::nullopt;
	}
}

static std::optional<std::string> FindShaderParameterMismatch(const ShaderParameterStructDescriptor& descriptor, const ShaderReflection& reflection, bool bAllowUnreflectedDeclarations)
{
	if (auto mismatch = ShaderParameterVerification::FindAutomaticValueMismatch(descriptor, reflection, bAllowUnreflectedDeclarations))
	{
		return mismatch;
	}
	return ShaderParameterVerification::FindDeclaredBindingMismatch(descriptor, reflection, bAllowUnreflectedDeclarations);
}

void VerifyCookedShaderParameters(const ShaderCompileJob& job, const CookedStageBuild& compiledStage)
{
	if (!job.Request.ParameterStruct.has_value())
	{
		return;
	}

	const auto mismatch = FindShaderParameterMismatch(*job.Request.ParameterStruct, compiledStage.reflection, job.Request.UnitKind == ShaderCompileUnitKind::Library);
	if (mismatch)
	{
		const auto jobDescription = FormatShaderCompileJob(job);
		const auto diagnostic = std::format("SC2001 {} parameter-struct '{}' verification failed: {}", jobDescription, job.Request.ParameterStruct->Name, *mismatch);
		throw Diagnostics::Error(diagnostic);
	}
}
