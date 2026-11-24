#include "PCH.h"

#include "ShaderParameters/PassParameterSet.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <algorithm>
#include <cassert>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_passParameterSetLogger, "Renderer.PassParameters");

template <typename THandle> static bool ValidateResourceBinding(std::span<const THandle> handles, const PassParameterDesc& parameter) noexcept
{
	if (parameter.ArrayCount != static_cast<std::uint32_t>(handles.size()))
	{
		return false;
	}

	return std::ranges::all_of(handles, [](const THandle& handle) { return handle.IsValid(); });
}

bool PassParameterBinding::IsBound() const noexcept
{
	return std::visit(
	    [](const auto& value) noexcept -> bool
	    {
		    using ValueType = std::decay_t<decltype(value)>;
		    if constexpr (std::is_same_v<ValueType, std::monostate>)
		    {
			    return false;
		    }
		    else if constexpr (std::is_same_v<ValueType, std::vector<FrameGraphBufferHandle>>)
		    {
			    return !value.empty();
		    }
		    else if constexpr (std::is_same_v<ValueType, RhiSamplerDesc>)
		    {
			    return true;
		    }
		    else if constexpr (std::is_same_v<ValueType, FrameGraphAccelerationStructureHandle>)
		    {
			    return value.IsValid();
		    }
		    else
		    {
			    return value.IsBound();
		    }
	    },
	    m_value);
}

PassParameterSet::PassParameterSet(const PassParameterLayout& layout, std::vector<bool> graphResourceParameters) :
    m_layout(layout),
    m_bindings(layout.GetParameterCount()),
    m_graphResourceParameters(std::move(graphResourceParameters))
{
	assert(m_graphResourceParameters.size() == layout.GetParameterCount());
}

void PassParameterSet::ClearBindings() noexcept
{
	for (PassParameterBinding& binding : m_bindings)
	{
		binding.m_value.emplace<std::monostate>();
	}
}

const PassParameterBinding* PassParameterSet::FindBinding(const char* name) const noexcept
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	return parameter != nullptr ? GetBinding(index) : nullptr;
}

const PassParameterBinding* PassParameterSet::GetBinding(std::uint32_t index) const noexcept
{
	return index < m_bindings.size() ? &m_bindings[index] : nullptr;
}

bool PassParameterSet::SetTexture(const char* name, FrameGraphTextureHandle handle)
{
	return SetTextureArray(name, std::span(&handle, 1));
}

bool PassParameterSet::SetTextureArray(const char* name, std::span<const FrameGraphTextureHandle> handles)
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	if (parameter == nullptr || parameter->ResourceDomain != ShaderParameterResourceDomain::Texture)
	{
		return false;
	}

	if (!ValidateResourceBinding(handles, *parameter))
	{
		return false;
	}

	m_bindings[index].m_value = PassParameterTextureBindingData{.Handles = {handles.begin(), handles.end()}};
	return true;
}

bool PassParameterSet::SetAttachment(const char* name, FrameGraphAttachmentBinding binding)
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	if (parameter == nullptr || parameter->ResourceDomain != ShaderParameterResourceDomain::Texture)
	{
		return false;
	}

	const bool bAttachment = parameter->Kind == ShaderParameterSemanticKind::RenderTarget || parameter->Kind == ShaderParameterSemanticKind::DepthTarget;
	if (!bAttachment || !binding.Handle.IsValid())
	{
		return false;
	}

	m_bindings[index].m_value = PassParameterTextureBindingData{.Attachment = binding};
	return true;
}

bool PassParameterSet::SetBuffer(const char* name, FrameGraphBufferHandle handle)
{
	return SetBufferArray(name, std::span(&handle, 1));
}

bool PassParameterSet::SetBufferArray(const char* name, std::span<const FrameGraphBufferHandle> handles)
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	if (parameter == nullptr || parameter->ResourceDomain != ShaderParameterResourceDomain::Buffer)
	{
		return false;
	}

	if (!ValidateResourceBinding(handles, *parameter))
	{
		return false;
	}

	m_bindings[index].m_value = std::vector<FrameGraphBufferHandle>{handles.begin(), handles.end()};
	return true;
}

bool PassParameterSet::SetShaderResourceView(const char* name, RhiDescriptorTableBinding descriptorTable)
{
	return SetDescriptorTable(name, PassParameterDescriptorTableBindingData{.Table = descriptorTable}, ShaderParameterSemanticKind::ReadTexture, ShaderParameterSemanticKind::ReadBuffer);
}

bool PassParameterSet::SetShaderResourceView(const char* name, RhiGpuDescriptorHandle descriptorTable)
{
	return SetDescriptorTable(name, PassParameterDescriptorTableBindingData{.GpuHandle = descriptorTable}, ShaderParameterSemanticKind::ReadTexture, ShaderParameterSemanticKind::ReadBuffer);
}

bool PassParameterSet::UsesGraphResource(std::uint32_t index) const noexcept
{
	return index < m_graphResourceParameters.size() && m_graphResourceParameters[index];
}

bool PassParameterSet::SetUnorderedAccessView(const char* name, RhiDescriptorTableBinding descriptorTable)
{
	return SetDescriptorTable(name, PassParameterDescriptorTableBindingData{.Table = descriptorTable}, ShaderParameterSemanticKind::RWTexture, ShaderParameterSemanticKind::RWBuffer);
}

bool PassParameterSet::SetUnorderedAccessView(const char* name, RhiGpuDescriptorHandle descriptorTable)
{
	return SetDescriptorTable(name, PassParameterDescriptorTableBindingData{.GpuHandle = descriptorTable}, ShaderParameterSemanticKind::RWTexture, ShaderParameterSemanticKind::RWBuffer);
}

bool PassParameterSet::SetAccelerationStructure(const char* name, FrameGraphAccelerationStructureHandle handle)
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	if (parameter == nullptr || parameter->Kind != ShaderParameterSemanticKind::AccelerationStructure)
	{
		return false;
	}

	if (parameter->ArrayCount != 1u || !handle.IsValid())
	{
		return false;
	}

	m_bindings[index].m_value = handle;
	return true;
}

bool PassParameterSet::SetUniformDataBytes(const char* name, const void* data, std::uint32_t sizeInBytes)
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	if (parameter == nullptr || parameter->ResourceDomain != ShaderParameterResourceDomain::Uniform)
	{
		return false;
	}

	if (data == nullptr || sizeInBytes == 0 || parameter->ValueSizeInBytes != sizeInBytes)
	{
		return false;
	}

	m_bindings[index].m_value = PassParameterUniformBindingData{.Data = data, .SizeInBytes = sizeInBytes};
	return true;
}

bool PassParameterSet::SetSampler(const char* name, RhiSamplerDesc sampler)
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	if (parameter == nullptr || parameter->ResourceDomain != ShaderParameterResourceDomain::Sampler)
	{
		return false;
	}

	m_bindings[index].m_value = sampler;
	return true;
}

bool PassParameterSet::HasAllRequiredBindings() const noexcept
{
	if (m_bindings.size() != m_layout.GetParameterCount())
	{
		return false;
	}

	for (const PassParameterBinding& binding : m_bindings)
	{
		if (!binding.IsBound())
		{
			return false;
		}
	}

	return true;
}

std::vector<std::string> PassParameterSet::GetMissingBindings() const
{
	std::vector<std::string> missing;
	const std::vector<PassParameterDesc>& parameters = m_layout.GetParameters();
	for (std::size_t index = 0; index < parameters.size() && index < m_bindings.size(); ++index)
	{
		if (!m_bindings[index].IsBound())
		{
			missing.push_back(parameters[index].Name);
		}
	}

	return missing;
}

[[noreturn]] void PassParameterSet::ReportInvalidBindings(const char* passName) const noexcept
{
	std::string message = "Shader pass '";
	message += passName != nullptr ? passName : "<unnamed>";
	message += "' has invalid parameters: ";
	const std::vector<std::string> missingBindings = GetMissingBindings();
	message += missingBindings.empty() ? "incomplete bindings" : "missing bindings ";
	for (std::size_t index = 0; index < missingBindings.size(); ++index)
	{
		if (index > 0)
		{
			message += ", ";
		}
		message += missingBindings[index];
	}
	message += ".";
	Diagnostics::Fatal(g_passParameterSetLogger, __FILE__, __LINE__, message);
}

const PassParameterDesc* PassParameterSet::FindParameter(const char* name, std::uint32_t& outIndex) const noexcept
{
	outIndex = 0;
	if (name == nullptr)
	{
		return nullptr;
	}

	const std::vector<PassParameterDesc>& parameters = m_layout.GetParameters();
	for (std::size_t index = 0; index < parameters.size(); ++index)
	{
		if (parameters[index].Name == name)
		{
			outIndex = static_cast<std::uint32_t>(index);
			return &parameters[index];
		}
	}

	return nullptr;
}

bool PassParameterSet::SetDescriptorTable(const char* name, PassParameterDescriptorTableBindingData binding, ShaderParameterSemanticKind textureKind, ShaderParameterSemanticKind bufferKind)
{
	std::uint32_t index = 0;
	const PassParameterDesc* parameter = FindParameter(name, index);
	if (parameter == nullptr || (parameter->Kind != textureKind && parameter->Kind != bufferKind) || !binding.IsBound())
	{
		return false;
	}

	m_bindings[index].m_value = binding;
	return true;
}
