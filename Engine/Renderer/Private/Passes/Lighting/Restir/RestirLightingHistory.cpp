#include "../../../PCH.h"
#include "Passes/Lighting/Restir/RestirLightingHistory.h"

#include "Core/Public/Hash/HashUtils.h"
#include "Frame/RenderFrame.h"
#include "Passes/Lighting/LightingSceneState.h"
#include "Passes/Lighting/Direct/DirectLightingControls.h"
#include "Passes/Lighting/Restir/Indirect/IndirectLightingControls.h"
#include "Passes/Lighting/Shadows/DirectShadowControls.h"
#include "Providers/RendererImageProviderStack.h"
#include "RayTracing/Effects/Shadows/RayTracedShadowCVars.h"
#include "RayTracing/Effects/RestirLighting/RestirIndirectLightingCVars.h"
#include "Resources/History/FrameHistory.h"

static std::uint64_t BuildRestirLightingHistoryInvalidationHash(const PreparedRenderScene& scene) noexcept
{
	std::uint64_t hash = BuildLightingSceneInvalidationHash(scene);
	hash = Hash::ContinueFnv1a64Value(hash, CVarRayTracedShadowNormalBias.Get());
	hash = Hash::ContinueFnv1a64Value(hash, CVarRayTracedShadowMaxDistance.Get());
	hash = Hash::ContinueFnv1a64Value(hash, ResolveRestirIndirectBounceCount());
	hash = Hash::ContinueFnv1a64Value(hash, CVarRestirIndirectTemporalReuse.Get());
	hash = Hash::ContinueFnv1a64Value(hash, CVarRestirIndirectSpatialReuse.Get());
	hash = AppendDirectLightingHistoryInvalidationHash(hash);
	hash = AppendIndirectLightingHistoryInvalidationHash(hash);
	hash = Hash::ContinueFnv1a64Value(hash, CVarDirectShadows.Get());
	return Hash::FinalizeFnv1a64(hash);
}

void RestirLightingHistory::Update(FrameGraph& frameGraph, const FrameHistoryResourceLayout& history, const RenderFrame& frame, RendererImageProviderStack& imageProviders)
{
	if (frame.View.temporalUniform.HistoryValid == 0u)
	{
		m_invalidationHash.reset();
	}

	if (!IsDirectLightingAdmitted() && !IsIndirectLightingAdmitted())
	{
		return;
	}

	const std::uint64_t hash = BuildRestirLightingHistoryInvalidationHash(frame.PreparedScene);
	const bool invalidated = !m_invalidationHash || *m_invalidationHash != hash;
	m_invalidationHash = hash;
	if (invalidated)
	{
		InvalidateReservoirHistory(frameGraph, history.DirectLightReservoir);
		InvalidateReservoirHistory(frameGraph, history.RestirIndirectReservoir);
		imageProviders.ResetHistory();
	}
}
