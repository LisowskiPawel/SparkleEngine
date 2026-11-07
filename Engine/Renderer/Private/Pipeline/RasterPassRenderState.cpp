#include "PCH.h"
#include "Pipeline/RasterPassRenderState.h"

void RasterPassRenderState::SetOpaqueBlend() noexcept
{
	m_blend = {};
}

void RasterPassRenderState::SetDepthTest(CompareOp comparison) noexcept
{
	m_depth.bDepthEnable = true;
	m_depth.DepthFunc = comparison;
}

void RasterPassRenderState::SetDepthWrite(bool bEnabled) noexcept
{
	m_depth.bDepthWriteEnable = bEnabled;
}

void RasterPassRenderState::DisableStencil() noexcept
{
	m_stencil = {};
}
