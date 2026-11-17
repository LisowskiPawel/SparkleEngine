#pragma once

#include "../RHIAPI.h"
#include "../ShaderParameters/ShaderParameterSemantics.h"
#include "../Shaders/ShaderStage.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

class PassParameterLayout;
struct ResolvedShader;

enum class CompiledBindingType : std::uint8_t
{
	ConstantBuffer,
	ReadOnlyAddress,
	ReadWriteAddress,
	AccelerationStructure,
	ReadOnlyResourceTable,
	ReadWriteResourceTable,
	SamplerTable,
	PushConstants,
};

struct RhiBindingPoint
{
	// HLSL register space and SPIR-V descriptor set share Set; HLSL register and SPIR-V binding share Binding.
	std::uint32_t Set = 0;
	std::uint32_t Binding = 0;
};

struct CompiledBinding
{
	const char* Name = nullptr;
	CompiledBindingType Type = CompiledBindingType::ReadOnlyResourceTable;
	ShaderParameterSemanticKind SemanticKind = ShaderParameterSemanticKind::ReadTexture;
	std::uint32_t BindingIndex = 0;
	RhiBindingPoint BindingPoint = {};
	ShaderStageMask VisibilityMask = ShaderStageMask::None;
	std::uint32_t DescriptorCount = 0;
	std::uint32_t PushConstantCount = 0;
};

struct RenderBindingLayoutCompileDesc
{
	const PassParameterLayout* ParameterLayout = nullptr;
	std::span<const ResolvedShader> Shaders;
	bool bAllowInputAssemblerInputLayout = false;
	const wchar_t* DebugName = L"RHI_BindingLayout";
	bool bInlineUniformDataAsPushConstants = false;
};

class SPARKLE_RHI_API RenderBindingLayout
{
public:
	virtual ~RenderBindingLayout() noexcept;
	RenderBindingLayout(const RenderBindingLayout&) = delete;
	RenderBindingLayout& operator=(const RenderBindingLayout&) = delete;
	RenderBindingLayout(RenderBindingLayout&&) = delete;
	RenderBindingLayout& operator=(RenderBindingLayout&&) = delete;

	const PassParameterLayout& GetParameterLayout() const noexcept;
	const CompiledBinding* GetBindings() const noexcept;
	std::size_t GetBindingCount() const noexcept;
	const CompiledBinding* FindBinding(const char* name) const noexcept;

protected:
	RenderBindingLayout(const PassParameterLayout& parameterLayout, std::vector<CompiledBinding> bindings, std::vector<std::string> bindingNames) noexcept;

private:
	const PassParameterLayout* m_parameterLayout = nullptr;

	std::vector<CompiledBinding> m_bindings;
	std::vector<std::string> m_bindingNames;
};
