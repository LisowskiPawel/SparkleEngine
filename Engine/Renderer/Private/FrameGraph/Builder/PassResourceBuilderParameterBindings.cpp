#include "PCH.h"
#include "FrameGraph/Builder/PassResourceBuilder.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <cassert>
#include <format>
#include <string>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_passResourceBuilderLogger, "Renderer.FrameGraph");

void PassResourceBuilder::DeclareParameterUsages(const PassParameterSet& parameterSet) noexcept
{
	const PassParameterLayout& layout = parameterSet.GetLayout();
	const std::vector<PassParameterDesc>& parameters = layout.GetParameters();

	for (std::uint32_t index = 0; index < static_cast<std::uint32_t>(parameters.size()); ++index)
	{
		const PassParameterDesc& parameter = parameters[index];
		if (!parameterSet.UsesGraphResource(index))
		{
			continue;
		}

		const PassParameterBinding* binding = parameterSet.GetBinding(index);
		if (binding == nullptr || !binding->IsBound())
		{
			const std::string message = std::format("Graph pass '{}' has no binding for '{}'.", layout.GetDebugName(), parameter.Name);

			Diagnostics::Fatal(g_passResourceBuilderLogger, __FILE__, __LINE__, message);
		}

		if (binding->AsDescriptorTableData() != nullptr)
		{
			continue;
		}

		if (parameter.ResourceDomain == ShaderParameterResourceDomain::Texture)
		{
			DeclareTextureBinding(parameter, *binding);
			continue;
		}

		if (parameter.ResourceDomain == ShaderParameterResourceDomain::Buffer)
		{
			DeclareBufferBinding(parameter, *binding);
			continue;
		}

		if (parameter.ResourceDomain == ShaderParameterResourceDomain::AccelerationStructure)
		{
			DeclareAccelerationStructureBinding(parameter, *binding);
			continue;
		}

		assert(false);
	}
}

ResourceUsage PassResourceBuilder::GetFrameGraphUsage(const PassParameterDesc& parameter) noexcept
{
	switch (parameter.Kind)
	{
		case ShaderParameterSemanticKind::ReadTexture:
		case ShaderParameterSemanticKind::ReadBuffer:
			return ResourceUsage::ShaderRead;
		case ShaderParameterSemanticKind::RWTexture:
		case ShaderParameterSemanticKind::RWBuffer:
			return ResourceUsage::UnorderedAccess;
		case ShaderParameterSemanticKind::RenderTarget:
			return ResourceUsage::RenderTarget;
		case ShaderParameterSemanticKind::DepthTarget:
			return ResourceUsage::DepthWrite;
		case ShaderParameterSemanticKind::AccelerationStructure:
			return ResourceUsage::AccelerationStructureRead;
		case ShaderParameterSemanticKind::UniformData:
		case ShaderParameterSemanticKind::SamplerSet:
		default:
			assert(false);
			return ResourceUsage::ShaderRead;
	}
}

void PassResourceBuilder::DeclareAccelerationStructureBinding(const PassParameterDesc& parameter, const PassParameterBinding& binding) noexcept
{
	const FrameGraphAccelerationStructureHandle* accelerationStructure = binding.AsAccelerationStructureHandle();
	assert(accelerationStructure != nullptr);
	assert(accelerationStructure->IsValid());

	DeclareResourceHandle(accelerationStructure->GetResourceHandle(), GetFrameGraphUsage(parameter), parameter, 0);
}

void PassResourceBuilder::DeclareTextureBinding(const PassParameterDesc& parameter, const PassParameterBinding& binding) noexcept
{
	const PassParameterTextureBindingData* textureData = binding.AsTextureData();
	assert(textureData != nullptr);

	const ResourceUsage usage = parameter.Kind == ShaderParameterSemanticKind::DepthTarget && textureData->IsAttachment()
	        && textureData->Attachment.DepthStencilAccess == FrameGraphDepthStencilAccess::ReadOnly
	    ? ResourceUsage::DepthRead
	    : GetFrameGraphUsage(parameter);

	if (textureData->IsAttachment())
	{
		DeclareResourceHandle(textureData->Attachment.Handle.GetResourceHandle(), usage, parameter, 0);
		return;
	}
	for (std::uint32_t arrayIndex = 0; arrayIndex < static_cast<std::uint32_t>(textureData->Handles.size()); ++arrayIndex)
	{
		DeclareResourceHandle(textureData->Handles[arrayIndex].GetResourceHandle(), usage, parameter, arrayIndex);
	}
}

void PassResourceBuilder::DeclareBufferBinding(const PassParameterDesc& parameter, const PassParameterBinding& binding) noexcept
{
	const PassParameterBufferBindingData* bufferData = binding.AsBufferData();
	assert(bufferData != nullptr);

	const ResourceUsage usage = GetFrameGraphUsage(parameter);
	for (std::uint32_t arrayIndex = 0; arrayIndex < static_cast<std::uint32_t>(bufferData->Handles.size()); ++arrayIndex)
	{
		DeclareResourceHandle(bufferData->Handles[arrayIndex].GetResourceHandle(), usage, parameter, arrayIndex);
	}
}

void PassResourceBuilder::DeclareResourceHandle(FrameGraphResourceHandle handle, ResourceUsage usage, const PassParameterDesc& parameter, std::uint32_t arrayIndex) noexcept
{
	std::string label = parameter.Name;
	if (parameter.ArrayCount > 1)
	{
		label += "[";
		label += std::to_string(arrayIndex);
		label += "]";
	}

	if (IsReadOnlyUsage(usage))
	{
		Read(handle, usage, label);
		return;
	}

	if (IsWriteOnlyUsage(usage))
	{
		Write(handle, usage, label);
		return;
	}

	Use(handle, usage, label);
}
