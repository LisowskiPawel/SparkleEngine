#include "PCH.h"

#include "Slang/ShaderBackendRegistration.h"

#include "Slang/SlangShaderBackend.h"

#include <array>

static constexpr std::array<std::string_view, 1> kSlangSourceExtensions = {{".slang"}};

static constexpr std::array<ShaderTarget, 11> kSlangCodegenTargets = {{
    ShaderTarget::DxilSm60,
    ShaderTarget::DxilSm61,
    ShaderTarget::DxilSm62,
    ShaderTarget::DxilSm63,
    ShaderTarget::DxilSm64,
    ShaderTarget::DxilSm65,
    ShaderTarget::DxilSm66,
    ShaderTarget::DxilSm67,
    ShaderTarget::SpirV14,
    ShaderTarget::SpirV15,
    ShaderTarget::SpirV16,
}};

static constexpr std::array<std::string_view, 2> kSlangBinaryFormats = {{"Dxil", "SpirV"}};

static std::unique_ptr<IShaderBackend> CreateBackendInstance()
{
	return std::make_unique<SlangShaderBackend>();
}

static bool QueryBackendAvailability(std::string& outUnavailableReason)
{
	outUnavailableReason.clear();
	return true;
}

ShaderBackendRegistration GetSlangBackendRegistration() noexcept
{
	return ShaderBackendRegistration{
	    .Descriptor =
	        ShaderBackendStaticDescriptor{
	            .Name = "slang",
	            .SourceExtensions = std::span<const std::string_view>(kSlangSourceExtensions.data(), kSlangSourceExtensions.size()),
	            .CodegenTargets = std::span<const ShaderTarget>(kSlangCodegenTargets.data(), kSlangCodegenTargets.size()),
	            .BinaryFormats = std::span<const std::string_view>(kSlangBinaryFormats.data(), kSlangBinaryFormats.size()),
	            .Capabilities = SlangShaderBackend::GetStaticCapabilities(),
	            .QueryVersion = &SlangShaderBackend::QueryBackendVersion,
	            .QueryAvailability = &QueryBackendAvailability,
	        },
	    .create = &CreateBackendInstance,
	};
}

struct SlangBackendRegistrar final
{
	SlangBackendRegistrar() { RegisterBuiltinShaderBackend(GetSlangBackendRegistration()); }
};

static SlangBackendRegistrar g_slangBackendRegistrar;
