#include "PCH.h"

#include "Shaders/ShaderMap.h"

#include "Core/Public/Hash/HashUtils.h"
#include "ShaderParameters/PassParameterLayout.h"

ShaderTypeId BuildShaderTypeId(std::string_view shaderName) noexcept
{
	constexpr std::string_view shaderTypeDomain = "Sparkle.ShaderType|";
	std::uint64_t hash = Hash::ContinueFnv1a64(Hash::kFnv64OffsetBasis, shaderTypeDomain.data(), shaderTypeDomain.size());
	hash = Hash::ContinueFnv1a64(hash, shaderName.data(), shaderName.size());
	return Hash::FinalizeFnv1a64(hash);
}

ShaderParameterSignature BuildShaderParameterSignature(const PassParameterLayout& layout) noexcept
{
	std::uint64_t hash = Hash::kFnv64OffsetBasis;
	for (const PassParameterDesc& parameter : layout.GetParameters())
	{
		hash = Hash::ContinueFnv1a64(hash, parameter.Name.data(), parameter.Name.size());
		hash = Hash::ContinueFnv1a64Value(hash, parameter.Kind);
		hash = Hash::ContinueFnv1a64Value(hash, parameter.ResourceDomain);
		hash = Hash::ContinueFnv1a64Value(hash, parameter.Access);
		hash = Hash::ContinueFnv1a64Value(hash, parameter.Visibility);
		hash = Hash::ContinueFnv1a64Value(hash, parameter.ArrayCount);
		hash = Hash::ContinueFnv1a64Value(hash, parameter.ValueSizeInBytes);
		hash = Hash::ContinueFnv1a64Value(hash, parameter.ValueLayoutHash);
	}
	return Hash::FinalizeFnv1a64(hash);
}

ShaderFeatureFlags operator|(ShaderFeatureFlags lhs, ShaderFeatureFlags rhs) noexcept
{
	return static_cast<ShaderFeatureFlags>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
}

ShaderFeatureFlags& operator|=(ShaderFeatureFlags& lhs, ShaderFeatureFlags rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}

bool HasShaderFeature(ShaderFeatureFlags value, ShaderFeatureFlags flag) noexcept
{
	return (static_cast<std::uint32_t>(value) & static_cast<std::uint32_t>(flag)) == static_cast<std::uint32_t>(flag);
}
