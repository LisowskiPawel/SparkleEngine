#include "PCH.h"

#include "Backend/ShaderBackendFactory.h"
#include "Backend/Dxc/DxcShaderBackend.h"
#include "Backend/Slang/SlangShaderBackend.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <array>
#include <cctype>

struct ShaderBackendEntry final
{
	std::string_view Name;
	std::string_view SourceExtension;
	ShaderBackendCapabilities Capabilities;
	std::uint64_t (*QueryVersion)();
	std::unique_ptr<IShaderBackend> (*Create)();
};

template <typename TBackend> static std::unique_ptr<IShaderBackend> CreateBackend()
{
	return std::make_unique<TBackend>();
}

static const std::array<ShaderBackendEntry, 2> kShaderBackends = {{
    {"dxc", ".hlsl", DxcShaderBackend::GetStaticCapabilities(), &DxcShaderBackend::QueryBackendVersion, &CreateBackend<DxcShaderBackend>},
    {"slang", ".slang", SlangShaderBackend::GetStaticCapabilities(), &SlangShaderBackend::QueryBackendVersion, &CreateBackend<SlangShaderBackend>},
}};

static constexpr std::array<std::string_view, 2> kShaderBinaryFormats = {"Dxil", "SpirV"};

static constexpr std::array<ShaderTarget, 11> kShaderCodegenTargets = {
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
};

static std::string NormalizeShaderBackendName(std::string_view name)
{
	std::string normalized(name);
	std::ranges::transform(normalized, normalized.begin(), [](unsigned char value) { return static_cast<char>(std::tolower(value)); });
	return normalized;
}

static std::string GetVirtualSourceExtension(std::string_view sourcePath)
{
	const std::size_t separator = sourcePath.find_last_of('/');
	const std::size_t period = sourcePath.find_last_of('.');
	if (period == std::string_view::npos || (separator != std::string_view::npos && period < separator))
	{
		return {};
	}

	return NormalizeShaderBackendName(sourcePath.substr(period));
}

static const ShaderBackendEntry& GetShaderBackend(std::string_view name)
{
	const std::string normalized = NormalizeShaderBackendName(name);
	const auto backend = std::ranges::find(kShaderBackends, normalized, &ShaderBackendEntry::Name);
	if (backend == kShaderBackends.end())
	{
		throw Diagnostics::Error("Unknown shader backend '" + std::string(name) + "'.");
	}
	return *backend;
}

static std::string SelectAutomaticShaderBackendName(std::string_view sourcePath, ShaderTarget target)
{
	const std::string extension = GetVirtualSourceExtension(sourcePath);
	for (const ShaderBackendEntry& backend : kShaderBackends)
	{
		if (backend.SourceExtension == extension && backend.Capabilities.SupportsTarget(target))
		{
			return std::string(backend.Name);
		}
	}
	return {};
}

std::vector<ShaderBackendDescriptor> ListShaderBackends()
{
	std::vector<ShaderBackendDescriptor> descriptors;
	descriptors.reserve(kShaderBackends.size());
	for (const ShaderBackendEntry& backend : kShaderBackends)
	{
		descriptors.push_back(QueryShaderBackend(backend.Name));
	}
	return descriptors;
}

ShaderBackendDescriptor QueryShaderBackend(std::string_view name)
{
	const ShaderBackendEntry& backend = GetShaderBackend(name);
	return {.Name = backend.Name, .SourceExtension = backend.SourceExtension, .Capabilities = backend.Capabilities, .Version = backend.QueryVersion()};
}

std::span<const std::string_view> ListShaderBinaryFormats() noexcept
{
	return kShaderBinaryFormats;
}

std::span<const ShaderTarget> ListShaderCodegenTargets() noexcept
{
	return kShaderCodegenTargets;
}

std::unique_ptr<IShaderBackend> CreateShaderBackend(std::string_view name)
{
	const ShaderBackendEntry& backend = GetShaderBackend(name);
	return backend.Create();
}

std::string ResolveShaderBackendName(std::string_view sourcePath, ShaderTarget target, std::string_view requestedName)
{
	std::string selectedName = NormalizeShaderBackendName(requestedName);
	if (selectedName.empty() || selectedName == "auto")
	{
		selectedName = SelectAutomaticShaderBackendName(sourcePath, target);
		if (selectedName.empty())
		{
			throw Diagnostics::Error("Unable to auto-select a shader backend for source '" + std::string(sourcePath) + "' and target '" + GetShaderTargetName(target) + "'.");
		}
	}

	const ShaderBackendEntry& backend = GetShaderBackend(selectedName);
	if (!backend.Capabilities.SupportsTarget(target))
	{
		throw Diagnostics::Error("Shader backend '" + selectedName + "' does not support target '" + GetShaderTargetName(target) + "'.");
	}

	return std::string(backend.Name);
}
