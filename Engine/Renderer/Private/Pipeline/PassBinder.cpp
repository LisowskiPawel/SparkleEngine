#include "../PCH.h"
#include "Pipeline/PassBinder.h"

#include "Commands/RenderCommandContext.h"
#include "FrameGraph/Execution/FrameGraphResourceCommands.h"
#include "ShaderParameters/PassParameterSet.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"

#include <string>
#include <string_view>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_passBinderLogger, "Renderer.PassBinder");

enum class BindingDomain : std::uint8_t
{
	Graphics,
	Compute,
	RayTracing,
};

struct BindingRequest;

static void BindPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout,
    BindingDomain domain);
static void BindCompiledBinding(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const CompiledBinding& compiledBinding,
    const PassParameterBinding* parameterBinding,
    const PassBindingOverrides* overrides,
    BindingDomain domain);
static void BindConstantBuffer(const BindingRequest& request);
static void BindReadOnlyAddress(const BindingRequest& request);
static void BindReadWriteAddress(const BindingRequest& request);
static void BindAccelerationStructure(const BindingRequest& request);
static void BindResourceTable(const BindingRequest& request, bool readWrite);
static void BindSamplerTable(const BindingRequest& request);
static void BindPushConstantData(const BindingRequest& request);
static void BindGpuAddress(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, RhiGpuVirtualAddress gpuAddress, BindingDomain domain);
static void BindDescriptorTableOverride(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, const PassBindingOverride& bindingOverride, BindingDomain domain);
static void BindDescriptorTable(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, RhiGpuDescriptorHandle descriptorTable, BindingDomain domain);
static void BindDescriptorTable(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, RhiDescriptorTableBinding descriptorTable, BindingDomain domain);
static void BindPushConstants(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, const void* data, std::uint32_t constantCount, BindingDomain domain);
static void Require(bool condition, std::string_view message);

struct BindingRequest final
{
	RenderCommandContext& CommandContext;
	const FrameGraphResourceCommands& Resources;
	RenderHardwareInterface& HardwareInterface;
	const CompiledBinding& Binding;
	const PassParameterBinding* Parameters;
	const PassBindingOverrides* Overrides;
	BindingDomain Domain = BindingDomain::Graphics;
};

void BindGraphicsPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout)
{
	BindPassParameters(commandContext, resources, layout, parameterSet, bindingNames, overrides, bindLayout, BindingDomain::Graphics);
}

void BindComputePassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout)
{
	BindPassParameters(commandContext, resources, layout, parameterSet, bindingNames, overrides, bindLayout, BindingDomain::Compute);
}

void BindRayTracingPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout)
{
	BindPassParameters(commandContext, resources, layout, parameterSet, bindingNames, overrides, bindLayout, BindingDomain::RayTracing);
}

static void ValidateParameterLayout(const RenderBindingLayout& layout, const PassParameterSet& parameterSet, std::span<const char* const> bindingNames)
{
	Require(parameterSet.HasLayout(), "Pass binding requires a parameter set with a compiled layout.");
	if (bindingNames.empty())
	{
		const PassParameterLayout* parameterLayout = parameterSet.GetLayout();
		const PassParameterLayout& compiledLayout = layout.GetParameterLayout();
		Require(parameterLayout != nullptr, "Pass parameter layout is unavailable.");
		Require(parameterLayout->Matches(compiledLayout), "Pass parameter layout does not match the compiled binding layout.");
	}
}

static void BindDomainLayout(RenderCommandContext& commandContext, const RenderBindingLayout& layout, BindingDomain domain)
{
	if (domain == BindingDomain::Compute)
	{
		commandContext.SetComputeBindingLayout(layout);
	}
	else if (domain == BindingDomain::RayTracing)
	{
		commandContext.SetRayTracingBindingLayout(layout);
	}
	else
	{
		commandContext.SetGraphicsBindingLayout(layout);
	}
}

static void BindNamedParameters(RenderCommandContext& commandContext, const FrameGraphResourceCommands& resources, const RenderBindingLayout& layout, const PassParameterSet& parameterSet, std::span<const char* const> bindingNames, const PassBindingOverrides* overrides, BindingDomain domain)
{
	for (const char* bindingName : bindingNames)
	{
		bool boundAny = false;
		for (std::size_t bindingIndex = 0; bindingIndex < layout.GetBindingCount(); ++bindingIndex)
		{
			const CompiledBinding& compiledBinding = layout.GetBindings()[bindingIndex];
			if (compiledBinding.Name == nullptr || std::string_view(compiledBinding.Name) != bindingName)
			{
				continue;
			}

			BindCompiledBinding(commandContext, resources, compiledBinding, parameterSet.FindBinding(bindingName), overrides, domain);
			boundAny = true;
		}

		Require(boundAny, "A requested pass binding name is absent from the compiled binding layout.");
	}
}

static void BindPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout,
    BindingDomain domain)
{
	ValidateParameterLayout(layout, parameterSet, bindingNames);
	if (bindLayout)
	{
		BindDomainLayout(commandContext, layout, domain);
	}

	if (bindingNames.empty())
	{
		for (std::size_t bindingIndex = 0; bindingIndex < layout.GetBindingCount(); ++bindingIndex)
		{
			const CompiledBinding& compiledBinding = layout.GetBindings()[bindingIndex];
			BindCompiledBinding(commandContext, resources, compiledBinding, parameterSet.FindBinding(compiledBinding.Name), overrides, domain);
		}
		return;
	}

	BindNamedParameters(commandContext, resources, layout, parameterSet, bindingNames, overrides, domain);
}

static void BindCompiledBinding(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const CompiledBinding& compiledBinding,
    const PassParameterBinding* parameterBinding,
    const PassBindingOverrides* overrides,
    BindingDomain domain)
{
	const BindingRequest request{
	    .CommandContext = commandContext,
	    .Resources = resources,
	    .HardwareInterface = resources.GetRenderHardwareInterface(),
	    .Binding = compiledBinding,
	    .Parameters = parameterBinding,
	    .Overrides = overrides,
	    .Domain = domain};

	switch (compiledBinding.Type)
	{
		case CompiledBindingType::ConstantBuffer:
			BindConstantBuffer(request);
			return;
		case CompiledBindingType::ReadOnlyAddress:
			BindReadOnlyAddress(request);
			return;
		case CompiledBindingType::ReadWriteAddress:
			BindReadWriteAddress(request);
			return;
		case CompiledBindingType::AccelerationStructure:
			BindAccelerationStructure(request);
			return;
		case CompiledBindingType::ReadOnlyResourceTable:
			BindResourceTable(request, false);
			return;
		case CompiledBindingType::ReadWriteResourceTable:
			BindResourceTable(request, true);
			return;
		case CompiledBindingType::SamplerTable:
			BindSamplerTable(request);
			return;
		case CompiledBindingType::PushConstants:
			BindPushConstantData(request);
			return;
		default:
			Diagnostics::Fatal(g_passBinderLogger, __FILE__, __LINE__, "Compiled pass binding has an unsupported type.");
	}
}

static void BindConstantBuffer(const BindingRequest& request)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::ConstantBufferView) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindGpuAddress(request.CommandContext, request.Binding, bindingOverride->GpuAddress, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, "Constant-buffer binding is absent from the pass parameter set.");
	const PassParameterUniformBindingData* uniformData = request.Parameters->AsUniformData();
	Require(uniformData != nullptr, "Constant-buffer binding has incompatible parameter data.");

	const RhiGpuVirtualAddress gpuAddress = request.HardwareInterface.GetUploadService().AllocateUniformConstantBuffer(
	    request.CommandContext.GetRenderCommandList(),
	    uniformData->Data,
	    uniformData->SizeInBytes);

	BindGpuAddress(request.CommandContext, request.Binding, gpuAddress, request.Domain);
}

static void BindReadOnlyAddress(const BindingRequest& request)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::ShaderResourceView) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindGpuAddress(request.CommandContext, request.Binding, bindingOverride->GpuAddress, request.Domain);
		return;
	}

	Require(false, "Read-only address binding requires an explicit pass override.");
}

static void BindReadWriteAddress(const BindingRequest& request)
{
	Require(request.Overrides != nullptr, "Read-write address binding requires an explicit pass override.");
	const PassBindingOverride* bindingOverride = request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::UnorderedAccessView);
	Require(bindingOverride != nullptr, "Read-write address binding has no unordered-access override.");
	BindGpuAddress(request.CommandContext, request.Binding, bindingOverride->GpuAddress, request.Domain);
}

static void BindAccelerationStructure(const BindingRequest& request)
{
	Require(request.Parameters != nullptr, "Acceleration-structure binding is absent from the pass parameter set.");
	const FrameGraphAccelerationStructureHandle* accelerationStructure = request.Parameters->AsAccelerationStructureHandle();
	Require(accelerationStructure != nullptr, "Acceleration-structure binding has incompatible parameter data.");
	const RhiResourceHandle resource = request.Resources.ResolveAccelerationStructure(*accelerationStructure);
	Require(static_cast<bool>(resource), "Acceleration-structure binding resolved to no resource.");
	if (request.Domain == BindingDomain::Compute)
	{
		request.CommandContext.BindComputeAccelerationStructure(request.Binding.BindingIndex, resource);
		return;
	}
	if (request.Domain == BindingDomain::RayTracing)
	{
		request.CommandContext.BindRayTracingAccelerationStructure(request.Binding.BindingIndex, resource);
		return;
	}
	request.CommandContext.BindAccelerationStructure(request.Binding.BindingIndex, resource);
}

static void BindResourceTable(const BindingRequest& request, bool readWrite)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::DescriptorTable) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindDescriptorTableOverride(request.CommandContext, request.Binding, *bindingOverride, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, readWrite ? "Read-write resource binding is absent from the pass parameter set." : "Read-only resource binding is absent from the pass parameter set.");
	if (const PassParameterDescriptorTableBindingData* descriptorTableData = request.Parameters->AsDescriptorTableData())
	{
		if (descriptorTableData->Table)
		{
			BindDescriptorTable(request.CommandContext, request.Binding, descriptorTableData->Table, request.Domain);
		}
		else
		{
			BindDescriptorTable(request.CommandContext, request.Binding, descriptorTableData->GpuHandle, request.Domain);
		}
		return;
	}

	if (const PassParameterTextureBindingData* textureData = request.Parameters->AsTextureData())
	{
		Require(!textureData->IsAttachment(), "Graph attachments cannot be bound as shader resource tables.");
		Require(textureData->Handles.size() == 1, "Texture binding must contain exactly one resource.");
		const RhiGpuDescriptorHandle view = readWrite ? request.Resources.ResolveUnorderedAccessView(textureData->Handles[0]) : request.Resources.ResolveShaderResourceView(textureData->Handles[0]);
		BindDescriptorTable(request.CommandContext, request.Binding, view, request.Domain);
		return;
	}

	const PassParameterBufferBindingData* bufferData = request.Parameters->AsBufferData();
	Require(bufferData != nullptr, readWrite ? "Read-write resource binding has incompatible parameter data." : "Read-only resource binding has incompatible parameter data.");
	Require(bufferData->Handles.size() == 1, "Buffer binding must contain exactly one resource.");
	const RhiGpuDescriptorHandle view = readWrite ? request.Resources.ResolveUnorderedAccessView(bufferData->Handles[0]) : request.Resources.ResolveShaderResourceView(bufferData->Handles[0]);
	BindDescriptorTable(request.CommandContext, request.Binding, view, request.Domain);
}

static void BindSamplerTable(const BindingRequest& request)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::DescriptorTable) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindDescriptorTableOverride(request.CommandContext, request.Binding, *bindingOverride, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, "Sampler binding is absent from the pass parameter set.");
	const PassParameterSamplerBindingData* samplerData = request.Parameters->AsSamplerData();
	Require(samplerData != nullptr, "Sampler binding has incompatible parameter data.");
	const RhiDescriptorTableBinding samplerBinding = request.HardwareInterface.GetDescriptorService().GetSharedSamplerBinding(samplerData->Desc);
	Require(static_cast<bool>(samplerBinding), "Sampler binding did not resolve a descriptor table.");
	BindDescriptorTable(request.CommandContext, request.Binding, samplerBinding, request.Domain);
}

static void BindPushConstantData(const BindingRequest& request)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::PushConstants) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindPushConstants(request.CommandContext, request.Binding, bindingOverride->ConstantsData, bindingOverride->ConstantCount, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, "Push-constant binding is absent from the pass parameter set.");
	const PassParameterUniformBindingData* uniformData = request.Parameters->AsUniformData();
	Require(uniformData != nullptr, "Push-constant binding has incompatible parameter data.");
	BindPushConstants(request.CommandContext, request.Binding, uniformData->Data, uniformData->SizeInBytes / static_cast<std::uint32_t>(sizeof(std::uint32_t)), request.Domain);
}

static void Require(bool condition, std::string_view message)
{
	if (!condition)
	{
		Diagnostics::Fatal(g_passBinderLogger, __FILE__, __LINE__, std::string(message));
	}
}

static void BindDescriptorTableOverride(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, const PassBindingOverride& bindingOverride, BindingDomain domain)
{
	if (bindingOverride.DescriptorTableKind == DescriptorTableOverrideKind::LogicalTable)
	{
		BindDescriptorTable(commandContext, compiledBinding, bindingOverride.LogicalDescriptorTable, domain);
	}
	else
	{
		BindDescriptorTable(commandContext, compiledBinding, bindingOverride.DescriptorTable, domain);
	}
}

static void BindGpuAddress(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, RhiGpuVirtualAddress gpuAddress, BindingDomain domain)
{
	Require(gpuAddress != 0, "Pass address binding resolved to a null GPU address.");
	switch (compiledBinding.Type)
	{
		case CompiledBindingType::ConstantBuffer:
			if (domain == BindingDomain::Compute)
			{
				commandContext.BindComputeConstantBuffer(compiledBinding.BindingIndex, gpuAddress);
			}
			else if (domain == BindingDomain::RayTracing)
			{
				commandContext.BindRayTracingConstantBuffer(compiledBinding.BindingIndex, gpuAddress);
			}
			else
			{
				commandContext.BindConstantBuffer(compiledBinding.BindingIndex, gpuAddress);
			}
			return;
		case CompiledBindingType::ReadOnlyAddress:
			if (domain == BindingDomain::Compute)
			{
				commandContext.BindComputeShaderResourceAddress(compiledBinding.BindingIndex, gpuAddress);
			}
			else if (domain == BindingDomain::RayTracing)
			{
				commandContext.BindRayTracingShaderResourceAddress(compiledBinding.BindingIndex, gpuAddress);
			}
			else
			{
				commandContext.BindShaderResourceAddress(compiledBinding.BindingIndex, gpuAddress);
			}
			return;
		case CompiledBindingType::ReadWriteAddress:
			if (domain == BindingDomain::Compute)
			{
				commandContext.BindComputeUnorderedAccessAddress(compiledBinding.BindingIndex, gpuAddress);
			}
			else if (domain == BindingDomain::RayTracing)
			{
				commandContext.BindRayTracingUnorderedAccessAddress(compiledBinding.BindingIndex, gpuAddress);
			}
			else
			{
				commandContext.BindUnorderedAccessAddress(compiledBinding.BindingIndex, gpuAddress);
			}
			return;
		default:
			Require(false, "Pass address binding has an unsupported type.");
			return;
	}
}

static void BindDescriptorTable(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, RhiGpuDescriptorHandle descriptorTable, BindingDomain domain)
{
	Require(static_cast<bool>(descriptorTable), "Pass resource binding resolved to a null GPU descriptor handle.");
	if (domain == BindingDomain::Compute)
	{
		commandContext.BindComputeDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}
	if (domain == BindingDomain::RayTracing)
	{
		commandContext.BindRayTracingDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}

	commandContext.BindDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
}

static void BindDescriptorTable(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, RhiDescriptorTableBinding descriptorTable, BindingDomain domain)
{
	Require(static_cast<bool>(descriptorTable), "Pass resource binding resolved to an invalid logical descriptor table.");
	if (domain == BindingDomain::Compute)
	{
		commandContext.BindComputeDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}
	if (domain == BindingDomain::RayTracing)
	{
		commandContext.BindRayTracingDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}

	commandContext.BindDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
}

static void BindPushConstants(RenderCommandContext& commandContext, const CompiledBinding& compiledBinding, const void* data, std::uint32_t constantCount, BindingDomain domain)
{
	Require(data != nullptr, "Push-constant binding has no data.");
	Require(constantCount > 0, "Push-constant binding has no values.");

	if (domain == BindingDomain::Compute)
	{
		commandContext.SetComputePushConstants(compiledBinding.BindingIndex, constantCount, data, 0);
		return;
	}
	if (domain == BindingDomain::RayTracing)
	{
		commandContext.SetRayTracingPushConstants(compiledBinding.BindingIndex, constantCount, data, 0);
		return;
	}

	commandContext.SetPushConstants(compiledBinding.BindingIndex, constantCount, data, 0);
}
