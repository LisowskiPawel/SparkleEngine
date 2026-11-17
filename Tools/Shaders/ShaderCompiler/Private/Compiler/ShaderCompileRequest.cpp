#include "PCH.h"

#include "Compiler/ShaderCompileRequest.h"

bool ShaderDescriptorBindingRemap::operator==(const ShaderDescriptorBindingRemap&) const = default;

ShaderCompileFeatureFlags operator|(ShaderCompileFeatureFlags lhs, ShaderCompileFeatureFlags rhs) noexcept
{
	return static_cast<ShaderCompileFeatureFlags>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

ShaderCompileFeatureFlags& operator|=(ShaderCompileFeatureFlags& lhs, ShaderCompileFeatureFlags rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}

bool HasShaderCompileFeature(ShaderCompileFeatureFlags value, ShaderCompileFeatureFlags flag) noexcept
{
	return (static_cast<std::uint8_t>(value) & static_cast<std::uint8_t>(flag)) != 0;
}

ShaderCompileRequest::ShaderCompileRequest(const ShaderSourceMountTable& sourceMounts) noexcept :
    SourceMounts(sourceMounts)
{
}
