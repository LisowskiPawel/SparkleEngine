#include "PCH.h"

#include "Pipeline/RhiPipelineDesc.h"

bool RhiBlendState::operator==(const RhiBlendState& other) const noexcept
{
	if (bAlphaToCoverageEnable != other.bAlphaToCoverageEnable || bIndependentBlendEnable != other.bIndependentBlendEnable)
	{
		return false;
	}

	const std::size_t targetCount = bIndependentBlendEnable ? Targets.size() : 1;
	for (std::size_t index = 0; index < targetCount; ++index)
	{
		if (Targets[index] != other.Targets[index])
		{
			return false;
		}
	}
	return true;
}

bool RhiDepthState::IsValid() const noexcept
{
	return bDepthEnable || !bDepthWriteEnable;
}

bool RhiStencilState::HasWrites() const noexcept
{
	const bool bFrontFailures = FrontFaceStencilFailOp != RhiStencilOp::Keep || FrontFaceStencilDepthFailOp != RhiStencilOp::Keep;
	const bool bFrontWrites = bFrontFailures || FrontFaceStencilPassOp != RhiStencilOp::Keep;
	const bool bBackFailures = BackFaceStencilFailOp != RhiStencilOp::Keep || BackFaceStencilDepthFailOp != RhiStencilOp::Keep;
	const bool bBackWrites = bBackFailures || BackFaceStencilPassOp != RhiStencilOp::Keep;
	const bool bFacesWrite = bFrontWrites || bBackWrites;
	const bool bWritesEnabled = bStencilEnable && StencilWriteMask != 0;
	return bWritesEnabled && bFacesWrite;
}

bool RhiVertexInputDeclaration::operator==(const RhiVertexInputDeclaration& other) const noexcept
{
	if (BindingCount != other.BindingCount || ElementCount != other.ElementCount)
	{
		return false;
	}
	if (BindingCount > Bindings.size() || ElementCount > Elements.size())
	{
		return Bindings == other.Bindings && Elements == other.Elements;
	}

	for (std::uint32_t index = 0; index < BindingCount; ++index)
	{
		if (Bindings[index] != other.Bindings[index])
		{
			return false;
		}
	}
	for (std::uint32_t index = 0; index < ElementCount; ++index)
	{
		if (Elements[index] != other.Elements[index])
		{
			return false;
		}
	}
	return true;
}

bool GraphicsPipelineDesc::IsValidSampleCount(std::uint8_t sampleCount) noexcept
{
	const bool bSmallCount = sampleCount == 1 || sampleCount == 2;
	const bool bLargeCount = sampleCount == 4 || sampleCount == 8;
	return bSmallCount || bLargeCount;
}

RenderPipeline::RenderPipeline() noexcept = default;

RenderPipeline::~RenderPipeline() noexcept = default;
