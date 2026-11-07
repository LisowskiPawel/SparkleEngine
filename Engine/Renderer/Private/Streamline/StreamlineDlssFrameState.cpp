#include "../PCH.h"
#include "Streamline/StreamlineDlssFrameState.h"

StreamlineDlssFrameState::StreamlineDlssFrameState() noexcept :
    m_qualityMode(GetRequestedQualityMode())
{
}

EUpscalerQualityMode StreamlineDlssFrameState::GetRequestedQualityMode() const noexcept
{
	return CVarUpscalerQualityMode.Get();
}

RenderViewportExtent StreamlineDlssFrameState::StoreResolution(RenderViewportExtent outputExtent, RenderViewportExtent providerRenderExtent) noexcept
{
	m_resolvedOutputExtent = outputExtent;
	m_resolvedRenderExtent = providerRenderExtent;
	m_bFrameValid = false;
	return providerRenderExtent;
}

void StreamlineDlssFrameState::SetupFrame(const ImageProviderFrameInput& frameInput) noexcept
{
	m_frameInput = frameInput;
	const EUpscalerQualityMode qualityMode = GetRequestedQualityMode();
	if (qualityMode != m_qualityMode)
	{
		m_qualityMode = qualityMode;
		m_frameInput.bResetHistory = true;
	}

	const bool bOutputReady = m_resolvedRenderExtent.IsValid() && frameInput.OutputExtent == m_resolvedOutputExtent;
	m_bFrameValid = bOutputReady && frameInput.RenderExtent == m_resolvedRenderExtent;
}

void StreamlineDlssFrameState::Reset() noexcept
{
	m_frameInput = {};
	m_resolvedOutputExtent = {};
	m_resolvedRenderExtent = {};
	m_bFrameValid = false;
}
