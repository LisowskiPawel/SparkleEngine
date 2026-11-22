#pragma once

#include "Backend/ShaderBackendCapabilities.h"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

class IShaderBackend;

struct ShaderBackendDescriptor final
{
	std::string_view Name;
	std::string_view SourceExtension;
	ShaderBackendCapabilities Capabilities;
	std::uint64_t Version = 0;
};

std::vector<ShaderBackendDescriptor> ListShaderBackends();
ShaderBackendDescriptor QueryShaderBackend(std::string_view name);
std::span<const std::string_view> ListShaderBinaryFormats() noexcept;
std::span<const ShaderTarget> ListShaderCodegenTargets() noexcept;
std::unique_ptr<IShaderBackend> CreateShaderBackend(std::string_view name);
std::string ResolveShaderBackendName(std::string_view sourcePath, ShaderTarget target, std::string_view requestedName);
