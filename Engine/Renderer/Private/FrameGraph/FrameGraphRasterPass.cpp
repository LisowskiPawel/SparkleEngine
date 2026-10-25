#include "PCH.h"
#include "FrameGraph/FrameGraph.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Pipeline/RasterPassRenderState.h"

static bool StencilWrites(const RhiStencilState& stencil) noexcept
{
	return stencil.StencilEnable && stencil.StencilWriteMask != 0
		&& (stencil.FrontFaceStencilFailOp != RhiStencilOp::Keep || stencil.FrontFaceStencilDepthFailOp != RhiStencilOp::Keep || stencil.FrontFaceStencilPassOp != RhiStencilOp::Keep
			|| stencil.BackFaceStencilFailOp != RhiStencilOp::Keep || stencil.BackFaceStencilDepthFailOp != RhiStencilOp::Keep || stencil.BackFaceStencilPassOp != RhiStencilOp::Keep);
}

static const PassParameterTextureBindingData& ResolveRasterAttachmentBinding(const PassParameterSet& parameters, std::uint32_t parameterIndex)
{
	const PassParameterBinding* const binding = parameters.GetBinding(parameterIndex);
	const PassParameterTextureBindingData* const texture = binding != nullptr ? binding->AsTextureData() : nullptr;
	if (texture == nullptr || !texture->IsAttachment())
	{
		throw Diagnostics::Error("Raster pass attachment is not fully declared through the graph.");
	}
	if (texture->Attachment.Store == FrameGraphAttachmentStoreAction::Discard)
	{
		throw Diagnostics::Error("Raster attachment discard is not supported by the current graph execution contract.");
	}
	return *texture;
}

static const FrameGraphTextureDesc& ResolveRasterAttachmentDescription(const FrameGraphResourceRegistry& resources, const FrameGraphAttachmentBinding& attachment)
{
	const FrameGraphResourceHandle resource = attachment.Handle.GetResourceHandle();
	if (!resources.IsRegistered(resource))
	{
		throw Diagnostics::Error("Raster pass attachment is not registered in the frame graph.");
	}
	const FrameGraphTextureDesc& desc = resources.GetMetadata(resource).textureDesc;
	if (desc.format == PixelFormat::Unknown || desc.sampleCount == 0)
	{
		throw Diagnostics::Error("Raster pass attachment has no valid format or sample count.");
	}
	return desc;
}

static void AddColorAttachment(FrameGraphRasterPass& result, const FrameGraphAttachmentBinding& attachment, const FrameGraphTextureDesc& desc)
{
	if (desc.kind != FrameGraphTextureKind::Color || result.ColorCount >= result.Colors.size())
	{
		throw Diagnostics::Error("Raster color attachment has an incompatible graph texture description.");
	}
	result.Colors[result.ColorCount] = attachment;
	result.Compatibility.ColorFormats[result.ColorCount] = desc.format;
	++result.ColorCount;
}

static void AddDepthStencilAttachment(FrameGraphRasterPass& result, const FrameGraphAttachmentBinding& attachment, const FrameGraphTextureDesc& desc)
{
	if (result.HasDepthStencil || desc.kind != FrameGraphTextureKind::DepthStencil)
	{
		throw Diagnostics::Error("Raster pass must declare at most one compatible depth-stencil attachment.");
	}
	result.DepthStencil = attachment;
	result.HasDepthStencil = true;
	result.Compatibility.DepthStencilAttachmentFormat = desc.format;
}

static FrameGraphRasterPass CollectRasterAttachments(const PassParameterLayout& layout, const PassParameterSet& parameters, const FrameGraphResourceRegistry& resources)
{
	FrameGraphRasterPass result;
	for (std::uint32_t index = 0; index < layout.GetParameterCount(); ++index)
	{
		const PassParameterDesc& parameter = layout.GetParameters()[index];
		if (parameter.Kind != ShaderParameterSemanticKind::RenderTarget && parameter.Kind != ShaderParameterSemanticKind::DepthTarget)
		{
			continue;
		}

		const PassParameterTextureBindingData& texture = ResolveRasterAttachmentBinding(parameters, index);
		const FrameGraphTextureDesc& desc = ResolveRasterAttachmentDescription(resources, texture.Attachment);
		if (parameter.Kind == ShaderParameterSemanticKind::RenderTarget)
		{
			AddColorAttachment(result, texture.Attachment, desc);
			continue;
		}

		AddDepthStencilAttachment(result, texture.Attachment, desc);
	}
	return result;
}

static void ResolveColorSampleCount(FrameGraphRasterPass& result, const FrameGraphResourceRegistry& resources)
{
	for (std::uint32_t index = 0; index < result.ColorCount; ++index)
	{
		const FrameGraphTextureDesc& desc = resources.GetMetadata(result.Colors[index].Handle.GetResourceHandle()).textureDesc;
		if (index == 0)
		{
			result.Compatibility.SampleCount = desc.sampleCount;
		}
		else if (desc.sampleCount != result.Compatibility.SampleCount)
		{
			throw Diagnostics::Error("Raster color attachments use incompatible sample counts.");
		}
	}
}

static void ResolveDepthStencilCompatibility(FrameGraphRasterPass& result, const RasterPassRenderState& renderState, const FrameGraphResourceRegistry& resources)
{

	if (result.HasDepthStencil)
	{
		const FrameGraphTextureDesc& depthDesc = resources.GetMetadata(result.DepthStencil.Handle.GetResourceHandle()).textureDesc;
		if ((result.ColorCount != 0 && depthDesc.sampleCount != result.Compatibility.SampleCount)
		    || (result.DepthStencil.DepthStencilAccess == FrameGraphDepthStencilAccess::ReadOnly
		        && (result.DepthStencil.Load == FrameGraphAttachmentLoadAction::Clear || renderState.GetDepth().DepthWriteEnable
		            || StencilWrites(renderState.GetStencil()))))
		{
			throw Diagnostics::Error("Raster depth attachment is incompatible with pass depth, stencil, access, load, or sample state.");
		}
		result.Compatibility.SampleCount = depthDesc.sampleCount;
	}
	else if (renderState.GetDepth().DepthEnable || renderState.GetStencil().StencilEnable)
	{
		throw Diagnostics::Error("Raster depth or stencil state requires a graph depth-stencil attachment.");
	}
}

static void ValidateRasterPassState(const FrameGraphRasterPass& result, const RasterPassRenderState& renderState)
{

	if (!renderState.GetDepth().DepthEnable && renderState.GetDepth().DepthWriteEnable)
	{
		throw Diagnostics::Error("Raster depth writes require depth testing to be enabled.");
	}
	if (result.Compatibility.SampleCount != 1 && result.Compatibility.SampleCount != 2 && result.Compatibility.SampleCount != 4 && result.Compatibility.SampleCount != 8)
	{
		throw Diagnostics::Error("Raster pass attachments use an unsupported sample count.");
	}

	if (result.ColorCount == 0 && !result.HasDepthStencil)
	{
		throw Diagnostics::Error("Raster pass requires at least one graph attachment.");
	}
}

FrameGraphRasterPass FrameGraph::BuildRasterPass(const PassParameterSet& parameters, const RasterPassRenderState& renderState) const
{
	const PassParameterLayout* const layout = parameters.GetLayout();
	if (layout == nullptr)
	{
		throw Diagnostics::Error("Raster pass has no parameter layout.");
	}

	FrameGraphRasterPass result = CollectRasterAttachments(*layout, parameters, m_resourceRegistry);
	result.Compatibility.ColorCount = result.ColorCount;
	result.Compatibility.SampleCount = 1;
	ResolveColorSampleCount(result, m_resourceRegistry);
	ResolveDepthStencilCompatibility(result, renderState, m_resourceRegistry);
	ValidateRasterPassState(result, renderState);
	return result;
}
