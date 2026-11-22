#include "PCH.h"

#include "Compiler/SpirV/SpirVBindingNormalizer.h"

#include "Core/Public/Diagnostics/Error.h"

#include <spirv_reflect.h>

#include <algorithm>
#include <cstring>
#include <string>

void NormalizeSpirVBindings(std::vector<std::uint8_t>& bytecode, std::span<const ShaderDescriptorBindingRemap> remaps)
{
	if (bytecode.empty())
	{
		return;
	}

	spv_reflect::ShaderModule module(bytecode.size(), bytecode.data());
	if (module.GetResult() != SPV_REFLECT_RESULT_SUCCESS)
	{
		throw Diagnostics::Error("spvReflectCreateShaderModule failed");
	}

	std::uint32_t bindingCount = 0;
	SpvReflectResult result = module.EnumerateDescriptorBindings(&bindingCount, nullptr);
	std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount, nullptr);
	if (result == SPV_REFLECT_RESULT_SUCCESS && bindingCount > 0)
	{
		result = module.EnumerateDescriptorBindings(&bindingCount, bindings.data());
	}
	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		throw Diagnostics::Error("spvReflectEnumerateDescriptorBindings failed");
	}

	for (SpvReflectDescriptorBinding* binding : bindings)
	{
		if (binding == nullptr || binding->name == nullptr)
		{
			throw Diagnostics::Error("SPIR-V descriptor has no canonical binding name");
		}
		const auto remap = std::ranges::find_if(remaps, [binding](const ShaderDescriptorBindingRemap& candidate) { return candidate.Name == binding->name; });
		if (remap == remaps.end())
		{
			const std::string name = binding->name;
			throw Diagnostics::Error("SPIR-V descriptor has no unambiguous canonical binding: '" + name + "'");
		}
		result = module.ChangeDescriptorBindingNumbers(binding, remap->Binding, remap->Set);
		if (result != SPV_REFLECT_RESULT_SUCCESS)
		{
			throw Diagnostics::Error("spvReflectChangeDescriptorBindingNumbers failed for '" + remap->Name + "'");
		}
	}

	const std::uint32_t normalizedSize = module.GetCodeSize();
	const std::uint32_t* const normalizedCode = module.GetCode();
	if (normalizedCode == nullptr || normalizedSize == 0)
	{
		throw Diagnostics::Error("spvReflectGetCode returned empty bytecode");
	}
	bytecode.resize(normalizedSize);
	std::memcpy(bytecode.data(), normalizedCode, normalizedSize);
}
