#include "../PCH.h"
#include "Pipeline/PassBinder.h"
#include "RHI/Public/Descriptors/RhiDescriptorService.h"
#include "RHI/Public/Resources/RhiUploadService.h"

#include "RHI/Public/Commands/RenderCommandList.h"
#include "FrameGraph/Execution/FrameGraphResourceCommands.h"
#include "ShaderParameters/PassParameterSet.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"

#include <string>
#include <string_view>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_passBinderLogger, "Renderer.PassBinder");

struct BindingRequest;

static void BindCompiledPassParameter(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const CompiledBinding& compiledBinding,
    const PassParameterBinding* parameterBinding,
    const PassBindingOverrides* overrides,
    BindingDomain domain);
static void BindConstantBuffer(const BindingRequest& request);
static void BindReadOnlyAddress(const BindingRequest& request);
static void BindReadWriteAddress(const BindingRequest& request);
static void BindAccelerationStructure(const BindingRequest& request);
static void BindResourceTable(const BindingRequest& request, bool bReadWrite);
static void BindSamplerTable(const BindingRequest& request);
static void BindPushConstantData(const BindingRequest& request);
static void BindGpuAddress(RenderCommandList& commandList, const CompiledBinding& compiledBinding, RhiGpuVirtualAddress gpuAddress, BindingDomain domain);
static void BindDescriptorTableOverride(RenderCommandList& commandList, const CompiledBinding& compiledBinding, const PassBindingOverride& bindingOverride, BindingDomain domain);
static void BindDescriptorTable(RenderCommandList& commandList, const CompiledBinding& compiledBinding, RhiGpuDescriptorHandle descriptorTable, BindingDomain domain);
static void BindDescriptorTable(RenderCommandList& commandList, const CompiledBinding& compiledBinding, RhiDescriptorTableBinding descriptorTable, BindingDomain domain);
static void BindPushConstants(RenderCommandList& commandList, const CompiledBinding& compiledBinding, const void* data, std::uint32_t constantCount, BindingDomain domain);
static void Require(bool bCondition, std::string_view message);

struct BindingRequest final
{
	RenderCommandList& CommandList;
	const FrameGraphResourceCommands& Resources;
	RenderHardwareInterface& HardwareInterface;
	const CompiledBinding& Binding;
	const PassParameterBinding* Parameters;
	const PassBindingOverrides* Overrides;
	BindingDomain Domain = BindingDomain::Graphics;
};

static void BindDomainLayout(RenderCommandList& commandList, const RenderBindingLayout& layout, BindingDomain domain)
{
	if (domain == BindingDomain::Compute)
	{
		commandList.SetComputeBindingLayout(layout);
	}
	else if (domain == BindingDomain::RayTracing)
	{
		commandList.SetRayTracingBindingLayout(layout);
	}
	else
	{
		commandList.SetGraphicsBindingLayout(layout);
	}
}

void BindPassParameters(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    BindingDomain domain,
    const PassBindingOverrides* overrides)
{
	// Raster permutations consume a subset of the shared draw parameters.
	if (domain != BindingDomain::Graphics)
	{
		Require(parameterSet.GetLayout().Matches(layout.GetParameterLayout()), "Pass parameter layout does not match the compiled binding layout.");
	}

	BindDomainLayout(commandList, layout, domain);
	for (std::size_t bindingIndex = 0; bindingIndex < layout.GetBindingCount(); ++bindingIndex)
	{
		const CompiledBinding& compiledBinding = layout.GetBindings()[bindingIndex];
		BindCompiledPassParameter(commandList, resources, compiledBinding, parameterSet.FindBinding(compiledBinding.Name), overrides, domain);
	}
}

static void BindCompiledPassParameter(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const CompiledBinding& compiledBinding,
    const PassParameterBinding* parameterBinding,
    const PassBindingOverrides* overrides,
    BindingDomain domain)
{
	const BindingRequest request{
	    .CommandList = commandList,
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
		BindGpuAddress(request.CommandList, request.Binding, bindingOverride->GpuAddress, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, "Constant-buffer binding is absent from the pass parameter set.");
	const PassParameterUniformBindingData* uniformData = request.Parameters->AsUniformData();
	Require(uniformData != nullptr, "Constant-buffer binding has incompatible parameter data.");

	const RhiGpuVirtualAddress gpuAddress = request.HardwareInterface.GetUploadService().AllocateUniformConstantBuffer(request.CommandList, uniformData->Data, uniformData->SizeInBytes);

	BindGpuAddress(request.CommandList, request.Binding, gpuAddress, request.Domain);
}

static void BindReadOnlyAddress(const BindingRequest& request)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::ShaderResourceView) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindGpuAddress(request.CommandList, request.Binding, bindingOverride->GpuAddress, request.Domain);
		return;
	}

	Require(false, "Read-only address binding requires an explicit pass override.");
}

static void BindReadWriteAddress(const BindingRequest& request)
{
	Require(request.Overrides != nullptr, "Read-write address binding requires an explicit pass override.");
	const PassBindingOverride* bindingOverride = request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::UnorderedAccessView);
	Require(bindingOverride != nullptr, "Read-write address binding has no unordered-access override.");
	BindGpuAddress(request.CommandList, request.Binding, bindingOverride->GpuAddress, request.Domain);
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
		request.CommandList.BindComputeAccelerationStructure(request.Binding.BindingIndex, resource);
		return;
	}
	if (request.Domain == BindingDomain::RayTracing)
	{
		request.CommandList.BindRayTracingAccelerationStructure(request.Binding.BindingIndex, resource);
		return;
	}
	request.CommandList.BindGraphicsAccelerationStructure(request.Binding.BindingIndex, resource);
}

static void BindResourceTable(const BindingRequest& request, bool bReadWrite)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::DescriptorTable) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindDescriptorTableOverride(request.CommandList, request.Binding, *bindingOverride, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, bReadWrite ? "Read-write resource binding is absent from the pass parameter set." : "Read-only resource binding is absent from the pass parameter set.");
	if (const PassParameterDescriptorTableBindingData* descriptorTableData = request.Parameters->AsDescriptorTableData())
	{
		if (descriptorTableData->Table)
		{
			BindDescriptorTable(request.CommandList, request.Binding, descriptorTableData->Table, request.Domain);
		}
		else
		{
			BindDescriptorTable(request.CommandList, request.Binding, descriptorTableData->GpuHandle, request.Domain);
		}
		return;
	}

	if (const PassParameterTextureBindingData* textureData = request.Parameters->AsTextureData())
	{
		Require(!textureData->IsAttachment(), "Graph attachments cannot be bound as shader resource tables.");
		Require(textureData->Handles.size() == 1, "Texture binding must contain exactly one resource.");
		const RhiGpuDescriptorHandle view = bReadWrite ? request.Resources.ResolveUnorderedAccessView(textureData->Handles[0]) : request.Resources.ResolveShaderResourceView(textureData->Handles[0]);
		BindDescriptorTable(request.CommandList, request.Binding, view, request.Domain);
		return;
	}

	const std::vector<FrameGraphBufferHandle>* bufferHandles = request.Parameters->AsBufferHandles();
	Require(bufferHandles != nullptr, bReadWrite ? "Read-write resource binding has incompatible parameter data." : "Read-only resource binding has incompatible parameter data.");
	Require(bufferHandles->size() == 1, "Buffer binding must contain exactly one resource.");
	const RhiGpuDescriptorHandle view = bReadWrite ? request.Resources.ResolveUnorderedAccessView(bufferHandles->front()) : request.Resources.ResolveShaderResourceView(bufferHandles->front());
	BindDescriptorTable(request.CommandList, request.Binding, view, request.Domain);
}

static void BindSamplerTable(const BindingRequest& request)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::DescriptorTable) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindDescriptorTableOverride(request.CommandList, request.Binding, *bindingOverride, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, "Sampler binding is absent from the pass parameter set.");
	const RhiSamplerDesc* sampler = request.Parameters->AsSamplerDesc();
	Require(sampler != nullptr, "Sampler binding has incompatible parameter data.");
	const RhiDescriptorTableBinding samplerBinding = request.HardwareInterface.GetDescriptorService().GetSharedSamplerBinding(*sampler);
	Require(static_cast<bool>(samplerBinding), "Sampler binding did not resolve a descriptor table.");
	BindDescriptorTable(request.CommandList, request.Binding, samplerBinding, request.Domain);
}

static void BindPushConstantData(const BindingRequest& request)
{
	const PassBindingOverride* bindingOverride = request.Overrides != nullptr ? request.Overrides->Find(request.Binding.Name, PassBindingOverrideType::PushConstants) : nullptr;
	if (bindingOverride != nullptr)
	{
		BindPushConstants(request.CommandList, request.Binding, bindingOverride->ConstantsData, bindingOverride->ConstantCount, request.Domain);
		return;
	}

	Require(request.Parameters != nullptr, "Push-constant binding is absent from the pass parameter set.");
	const PassParameterUniformBindingData* uniformData = request.Parameters->AsUniformData();
	Require(uniformData != nullptr, "Push-constant binding has incompatible parameter data.");
	BindPushConstants(request.CommandList, request.Binding, uniformData->Data, uniformData->SizeInBytes / static_cast<std::uint32_t>(sizeof(std::uint32_t)), request.Domain);
}

static void Require(bool bCondition, std::string_view message)
{
	if (!bCondition)
	{
		Diagnostics::Fatal(g_passBinderLogger, __FILE__, __LINE__, message);
	}
}

static void BindDescriptorTableOverride(RenderCommandList& commandList, const CompiledBinding& compiledBinding, const PassBindingOverride& bindingOverride, BindingDomain domain)
{
	if (bindingOverride.DescriptorTableKind == DescriptorTableOverrideKind::LogicalTable)
	{
		BindDescriptorTable(commandList, compiledBinding, bindingOverride.LogicalDescriptorTable, domain);
	}
	else
	{
		BindDescriptorTable(commandList, compiledBinding, bindingOverride.DescriptorTable, domain);
	}
}

static void BindGpuAddress(RenderCommandList& commandList, const CompiledBinding& compiledBinding, RhiGpuVirtualAddress gpuAddress, BindingDomain domain)
{
	Require(gpuAddress != 0, "Pass address binding resolved to a null GPU address.");
	switch (compiledBinding.Type)
	{
		case CompiledBindingType::ConstantBuffer:
			if (domain == BindingDomain::Compute)
			{
				commandList.BindComputeConstantBuffer(compiledBinding.BindingIndex, gpuAddress);
			}
			else if (domain == BindingDomain::RayTracing)
			{
				commandList.BindRayTracingConstantBuffer(compiledBinding.BindingIndex, gpuAddress);
			}
			else
			{
				commandList.BindGraphicsConstantBuffer(compiledBinding.BindingIndex, gpuAddress);
			}
			return;
		case CompiledBindingType::ReadOnlyAddress:
			if (domain == BindingDomain::Compute)
			{
				commandList.BindComputeShaderResource(compiledBinding.BindingIndex, gpuAddress);
			}
			else if (domain == BindingDomain::RayTracing)
			{
				commandList.BindRayTracingShaderResource(compiledBinding.BindingIndex, gpuAddress);
			}
			else
			{
				commandList.BindGraphicsShaderResource(compiledBinding.BindingIndex, gpuAddress);
			}
			return;
		case CompiledBindingType::ReadWriteAddress:
			if (domain == BindingDomain::Compute)
			{
				commandList.BindComputeUnorderedAccess(compiledBinding.BindingIndex, gpuAddress);
			}
			else if (domain == BindingDomain::RayTracing)
			{
				commandList.BindRayTracingUnorderedAccess(compiledBinding.BindingIndex, gpuAddress);
			}
			else
			{
				commandList.BindGraphicsUnorderedAccess(compiledBinding.BindingIndex, gpuAddress);
			}
			return;
		default:
			Require(false, "Pass address binding has an unsupported type.");
			return;
	}
}

static void BindDescriptorTable(RenderCommandList& commandList, const CompiledBinding& compiledBinding, RhiGpuDescriptorHandle descriptorTable, BindingDomain domain)
{
	Require(static_cast<bool>(descriptorTable), "Pass resource binding resolved to a null GPU descriptor handle.");
	if (domain == BindingDomain::Compute)
	{
		commandList.BindComputeDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}
	if (domain == BindingDomain::RayTracing)
	{
		commandList.BindRayTracingDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}

	commandList.BindGraphicsDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
}

static void BindDescriptorTable(RenderCommandList& commandList, const CompiledBinding& compiledBinding, RhiDescriptorTableBinding descriptorTable, BindingDomain domain)
{
	Require(static_cast<bool>(descriptorTable), "Pass resource binding resolved to an invalid logical descriptor table.");
	if (domain == BindingDomain::Compute)
	{
		commandList.BindComputeDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}
	if (domain == BindingDomain::RayTracing)
	{
		commandList.BindRayTracingDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
		return;
	}

	commandList.BindGraphicsDescriptorTable(compiledBinding.BindingIndex, descriptorTable);
}

static void BindPushConstants(RenderCommandList& commandList, const CompiledBinding& compiledBinding, const void* data, std::uint32_t constantCount, BindingDomain domain)
{
	Require(data != nullptr, "Push-constant binding has no data.");
	Require(constantCount > 0, "Push-constant binding has no values.");

	if (domain == BindingDomain::Compute)
	{
		commandList.SetComputePushConstants(compiledBinding.BindingIndex, constantCount, data, 0);
		return;
	}
	if (domain == BindingDomain::RayTracing)
	{
		commandList.SetRayTracingPushConstants(compiledBinding.BindingIndex, constantCount, data, 0);
		return;
	}

	commandList.SetGraphicsPushConstants(compiledBinding.BindingIndex, constantCount, data, 0);
}
