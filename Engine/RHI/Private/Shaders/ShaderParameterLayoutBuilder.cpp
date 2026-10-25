#include "PCH.h"

#include "Shaders/ShaderParameterLayoutBuilder.h"

#include "Core/Public/Diagnostics/Error.h"
#include "ShaderParameters/PassParameterLayout.h"

#include <algorithm>
#include <format>
#include <string>
#include <utility>
#include <vector>

struct ParameterLayoutEntry final
{
	PassParameterDesc Parameter;
	std::uint32_t ValueAlignmentInBytes = 0;
	std::string ShaderName;
};

static PassParameterDesc BuildParameter(const ShaderParameterStructFieldDescriptor& field, ShaderStage stage)
{
	return PassParameterDesc{
	    .Name = field.Name,
	    .Kind = field.SemanticKind,
	    .ResourceDomain = field.ResourceDomain,
	    .Access = field.Access,
	    .Visibility = field.Visibility == ShaderStageVisibility::None ? GetDefaultShaderVisibility(stage) : field.Visibility,
	    .ArrayCount = field.ArrayCount,
	    .ValueSizeInBytes = field.ValueSizeInBytes,
	    .ValueLayoutHash = field.ValueLayoutHash};
}

static std::uint32_t Category(const PassParameterDesc& parameter) noexcept
{
	if (parameter.Kind == ShaderParameterSemanticKind::RenderTarget || parameter.Kind == ShaderParameterSemanticKind::DepthTarget)
	{
		return 0;
	}
	return parameter.Kind == ShaderParameterSemanticKind::SamplerSet ? 2u : 1u;
}

static bool Matches(const ParameterLayoutEntry& current, const PassParameterDesc& incoming, std::uint32_t alignment) noexcept
{
	return current.Parameter.Kind == incoming.Kind && current.Parameter.ResourceDomain == incoming.ResourceDomain && current.Parameter.Access == incoming.Access
	    && current.Parameter.ArrayCount == incoming.ArrayCount && current.Parameter.ValueSizeInBytes == incoming.ValueSizeInBytes && current.Parameter.ValueLayoutHash == incoming.ValueLayoutHash
	    && (current.Parameter.Kind != ShaderParameterSemanticKind::UniformData || current.ValueAlignmentInBytes == alignment);
}

ShaderStageVisibility GetDefaultShaderVisibility(ShaderStage stage) noexcept
{
	switch (stage)
	{
		case ShaderStage::Vertex:
			return ShaderStageVisibility::Vertex;
		case ShaderStage::Pixel:
			return ShaderStageVisibility::Pixel;
		case ShaderStage::Compute:
			return ShaderStageVisibility::Compute;
		case ShaderStage::RayGeneration:
		case ShaderStage::Miss:
		case ShaderStage::ClosestHit:
		case ShaderStage::AnyHit:
		case ShaderStage::Intersection:
		case ShaderStage::Callable:
			return ShaderStageVisibility::RayTracing;
		case ShaderStage::Geometry:
		case ShaderStage::Hull:
		case ShaderStage::Domain:
			return ShaderStageVisibility::AllGraphics;
		case ShaderStage::Count:
		default:
			return ShaderStageVisibility::None;
	}
}

static void AppendShaderParameters(const ShaderRegistrationDesc& shader, std::vector<ParameterLayoutEntry>& entries)
{
	if (shader.BuildParameterStructDescriptor == nullptr)
	{
		return;
	}
	const ShaderParameterStructDescriptor descriptor = shader.BuildParameterStructDescriptor();
	for (const ShaderParameterStructFieldDescriptor& field : descriptor.Fields)
	{
		PassParameterDesc parameter = BuildParameter(field, shader.Stage);
		if (parameter.Name.empty() || parameter.ArrayCount == 0)
		{
			throw Diagnostics::Error(std::format("Shader '{}' contains an invalid parameter declaration.", shader.ShaderName));
		}
		const auto existing = std::ranges::find_if(entries, [&parameter](const ParameterLayoutEntry& entry) { return entry.Parameter.Name == parameter.Name; });
		if (existing == entries.end())
		{
			entries.push_back(ParameterLayoutEntry{.Parameter = std::move(parameter), .ValueAlignmentInBytes = field.ValueAlignmentInBytes, .ShaderName = std::string(shader.ShaderName)});
		}
		else if (!Matches(*existing, parameter, field.ValueAlignmentInBytes))
		{
			throw Diagnostics::Error(std::format("Shaders '{}' and '{}' declare incompatible parameter '{}'.", existing->ShaderName, shader.ShaderName, parameter.Name));
		}
		else
		{
			existing->Parameter.Visibility |= parameter.Visibility;
		}
	}
}

PassParameterLayout BuildShaderPipelineParameterLayout(std::span<const ShaderRegistrationDesc* const> shaders)
{
	if (shaders.empty())
	{
		throw Diagnostics::Error("Shader parameter layout generation received no shader types.");
	}

	std::vector<ParameterLayoutEntry> entries;
	std::string debugName;
	for (const ShaderRegistrationDesc* shader : shaders)
	{
		if (shader == nullptr)
		{
			throw Diagnostics::Error("Shader parameter layout generation received an incomplete shader registration.");
		}
		if (!debugName.empty())
		{
			debugName += '+';
		}
		debugName += shader->ShaderName;
		AppendShaderParameters(*shader, entries);
	}

	PassParameterLayout layout(debugName.c_str());
	for (std::uint32_t category = 0; category < 3; ++category)
	{
		for (const ParameterLayoutEntry& entry : entries)
		{
			if (Category(entry.Parameter) == category)
			{
				layout.AddParameter(entry.Parameter);
			}
		}
	}
	return layout;
}

PassParameterLayout BuildShaderParameterLayout(const ShaderRegistrationDesc& shader)
{
	const ShaderRegistrationDesc* shaders[] = {&shader};
	return BuildShaderPipelineParameterLayout(shaders);
}
