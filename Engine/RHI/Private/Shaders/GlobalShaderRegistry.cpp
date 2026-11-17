#include "PCH.h"

#include "Shaders/Authoring/GlobalShader.h"

#include <algorithm>
#include <vector>

static std::vector<ShaderRegistrationDesc>& MutableGlobalShaderRegistrations()
{
	static std::vector<ShaderRegistrationDesc> registrations;
	return registrations;
}

static const std::vector<ShaderRegistrationDesc>& GlobalShaderRegistrationSnapshot() noexcept
{
	static const std::vector<ShaderRegistrationDesc> registrations = []
	{
		std::vector<ShaderRegistrationDesc> snapshot = MutableGlobalShaderRegistrations();
		std::ranges::sort(snapshot, [](const ShaderRegistrationDesc& left, const ShaderRegistrationDesc& right) { return left.TypeId < right.TypeId; });
		return snapshot;
	}();

	return registrations;
}

void GlobalShaderRegistry::Register(ShaderRegistrationDesc desc)
{
	MutableGlobalShaderRegistrations().push_back(desc);
}

std::span<const ShaderRegistrationDesc> GlobalShaderRegistry::GetRegistrations() noexcept
{
	return GlobalShaderRegistrationSnapshot();
}

const ShaderRegistrationDesc* GlobalShaderRegistry::FindByName(std::string_view shaderName) noexcept
{
	const std::span<const ShaderRegistrationDesc> registrations = GetRegistrations();
	const auto found = std::ranges::find_if(registrations, [shaderName](const ShaderRegistrationDesc& desc) { return desc.ShaderName == shaderName; });
	return found != registrations.end() ? &*found : nullptr;
}

const ShaderRegistrationDesc* GlobalShaderRegistry::FindById(ShaderTypeId shaderType) noexcept
{
	const std::span<const ShaderRegistrationDesc> registrations = GetRegistrations();
	const auto found = std::ranges::lower_bound(registrations, shaderType, {}, &ShaderRegistrationDesc::TypeId);
	return found != registrations.end() && found->TypeId == shaderType ? &*found : nullptr;
}

const ShaderRegistrationDesc* GlobalShaderRegistry::FindByType(const std::type_info& shaderType) noexcept
{
	const std::span<const ShaderRegistrationDesc> registrations = GetRegistrations();
	const auto found = std::ranges::find_if(registrations, [&shaderType](const ShaderRegistrationDesc& candidate) { return candidate.ShaderType != nullptr && *candidate.ShaderType == shaderType; });
	return found != registrations.end() ? &*found : nullptr;
}
