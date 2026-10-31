#include "PCH.h"

#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerSession.h"

#include "Core/Public/Hash/HashUtils.h"
#include "Frame/RenderFrame.h"
#include "RHI/Public/Core/RhiCapabilities.h"
#include "RHI/Public/Device/RenderDeviceServices.h"
#include "Scene/Materials/MaterialData.h"
#include "Scene/Materials/MaterialTextureTableCapability.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "View/RenderView.h"

#include <algorithm>

static_assert(static_cast<std::uint8_t>(ReferencePathTracerIdentityComponent::Backend) == static_cast<std::uint8_t>(ViewportRenderProgressReason::BackendChanged));

static bool HasIdealPunctualLightModel(const auto& light) noexcept
{
	return light.radius == 0.0f && light.range == 0.0f && light.distanceAttenuationCoefficients.x == 0.0f && light.distanceAttenuationCoefficients.y == 0.0f
	    && light.distanceAttenuationCoefficients.z == 1.0f;
}

static bool HasUnsupportedReferenceLightModel(const PreparedRenderScene& scene) noexcept
{
	for (std::size_t index = 0; index < scene.directionalLights.size(); ++index)
	{
		if (scene.directionalLights[index].angularSizeRadians != 0.0f)
		{
			return true;
		}
	}
	for (std::size_t index = 0; index < scene.pointLights.size(); ++index)
	{
		if (!HasIdealPunctualLightModel(scene.pointLights[index]))
		{
			return true;
		}
	}
	for (std::size_t index = 0; index < scene.spotLights.size(); ++index)
	{
		if (!HasIdealPunctualLightModel(scene.spotLights[index]))
		{
			return true;
		}
	}
	return false;
}

ViewportRenderProgressReason ReferencePathTracerSession::ResolveAvailability(const RenderView& view, const PreparedRenderScene& scene, RayTracingExecutionFrontend executionFrontend) const noexcept
{
	const RhiCapabilities& capabilities = m_deviceServices.GetCapabilities();
	const RhiFormatSupport* format = capabilities.FindFormatSupport(PixelFormat::R32G32B32A32_Float);
	const bool supportsView = view.camera.ProjectionKind == CameraProjectionKind::Perspective && view.renderExtent.IsValid() && view.renderExtent.Width <= 16384u && view.renderExtent.Height <= 16384u;
	if (!supportsView)
	{
		return ViewportRenderProgressReason::UnsupportedView;
	}

	const bool supportsAccumulation = format != nullptr && format->SupportsShaderResource && format->SupportsUnorderedAccess;

	if (!supportsAccumulation || executionFrontend == RayTracingExecutionFrontend::None || !SupportsMaterialTextureTable(capabilities))
	{
		return ViewportRenderProgressReason::UnsupportedCapability;
	}

	const bool unsupportedContent = std::any_of(
	    scene.materials.begin(),
	    scene.materials.end(),
	    [](const MaterialData& material) { return material.alphaMode == 2u || material.subsurfaceStrength != 0.0f; });

	return unsupportedContent || HasUnsupportedReferenceLightModel(scene) ? ViewportRenderProgressReason::UnsupportedContent : ViewportRenderProgressReason::None;
}

void ReferencePathTracerSession::BeginIdentity(const ReferencePathTracerIdentity& identity, RenderViewportExtent extent, ViewportRenderProgressReason reason, std::uint32_t discardedSamples) noexcept
{
	m_pendingCommit = {};
	m_identity = identity;
	Hash::Sha256Digest identityHash{};
	std::string hashError;
	if (Hash::TrySha256(identity.Components.data(), sizeof(identity.Components), identityHash, hashError))
	{
		m_identitySha256 = identityHash;
	}
	else
	{
		m_identitySha256 = {};
	}
	m_hasIdentity = true;
	m_committedSamples = 0u;
	m_discardedSamples = discardedSamples;
	m_pixelCount = static_cast<std::uint64_t>(extent.Width) * static_cast<std::uint64_t>(extent.Height);
	m_nextRow = 0u;
	m_workPrepared = false;
	m_clearDisplay = true;
	m_resetVisible = reason != ViewportRenderProgressReason::None;
	m_paused = m_paused || m_pausePending;
	m_pausePending = false;
	m_suspensionPending = false;
	m_samplesPerSecond = 0.0;
	m_lastCommitTime = std::chrono::steady_clock::now();
	m_lastReason = reason;
	m_unavailableReason = ViewportRenderProgressReason::None;
	++m_executionGeneration;
}

void ReferencePathTracerSession::UpdateIdentity(const RenderFrame& frame, RayTracingExecutionFrontend executionFrontend) noexcept
{
	const RenderView& view = frame.View;
	const PreparedRenderScene& scene = frame.PreparedScene;
	const ReferencePathTracerIdentity identity = BuildReferencePathTracerIdentity(frame, executionFrontend, m_deviceServices.GetCapabilities().BackendApi);
	if (m_hasIdentity && identity == m_identity)
	{
		return;
	}
	const ViewportRenderProgressReason reason = m_hasIdentity ? static_cast<ViewportRenderProgressReason>(m_identity.FindFirstDifference(identity)) : m_lastReason;
	const std::uint32_t discardedSamples = m_hasIdentity ? m_committedSamples : m_discardedSamples;
	const ViewportRenderProgressReason availability = ResolveAvailability(view, scene, executionFrontend);
	if (availability != ViewportRenderProgressReason::None)
	{
		m_committedSamples = 0u;
		m_discardedSamples = discardedSamples;
		m_pendingCommit = {};
		m_workPrepared = false;
		m_resetVisible = false;
		m_paused = false;
		m_pausePending = false;
		m_retentionAvailable = false;
		++m_executionGeneration;
		m_resources.Release();
		m_identity = identity;
		m_hasIdentity = true;
		m_hasOwner = false;
		m_lastReason = availability;
		m_unavailableReason = availability;
		return;
	}
	m_resources.Allocate(view.renderExtent);
	BeginIdentity(identity, view.renderExtent, reason, discardedSamples);
}

void ReferencePathTracerSession::ApplyAction(ViewportRenderAction action, std::uint64_t actionSequence, const RenderView& view) noexcept
{
	if (view.viewportId == m_lastActionViewportId && actionSequence == m_lastActionSequence)
	{
		return;
	}
	m_lastActionViewportId = view.viewportId;
	m_lastActionSequence = actionSequence;

	switch (action)
	{
		case ViewportRenderAction::Pause:
			if (m_unavailableReason == ViewportRenderProgressReason::None && m_hasIdentity && m_resources.CanRetain())
			{
				m_workPrepared = false;
				m_pausePending = static_cast<bool>(m_pendingCommit);
				m_paused = !m_pausePending;
			}
			break;
		case ViewportRenderAction::Resume:
			if (m_paused)
			{
				m_paused = false;
				m_lastReason = ViewportRenderProgressReason::Resumed;
				m_lastCommitTime = std::chrono::steady_clock::now();
			}
			break;
		case ViewportRenderAction::Restart:
			if (m_unavailableReason == ViewportRenderProgressReason::None && m_hasIdentity)
			{
				m_paused = false;
				BeginIdentity(m_identity, view.renderExtent, ViewportRenderProgressReason::ManualRestart, m_committedSamples);
			}
			break;
		case ViewportRenderAction::Transfer:
			if (m_hasOwner && m_ownerViewportId != view.viewportId)
			{
				m_transferViewportId = view.viewportId;
				m_transferPending = true;
			}
			break;
		case ViewportRenderAction::None:
		default:
			break;
	}
}

void ReferencePathTracerSession::Suspend() noexcept
{
	if (m_suspended)
	{
		if (m_suspensionPending && !m_pendingCommit)
		{
			FinalizeSuspension();
		}
		else if (m_hasIdentity && m_resources.IsAllocated() && !m_resources.CanRetain())
		{
			ReleaseRetainedIdentity();
		}
		return;
	}
	if (!m_hasIdentity && !m_resources.IsAllocated())
	{
		m_suspended = true;
		return;
	}
	m_suspended = true;
	m_workPrepared = false;
	m_nextRow = 0u;
	if (m_pendingCommit)
	{
		m_suspensionPending = true;
		return;
	}
	FinalizeSuspension();
}

void ReferencePathTracerSession::FinalizeSuspension() noexcept
{
	m_suspensionPending = false;
	++m_executionGeneration;
	if (!m_resources.CanRetain())
	{
		ReleaseRetainedIdentity();
	}
}

bool ReferencePathTracerSession::SelectViewport(const RenderView& view, std::uint64_t actionSequence) noexcept
{
	if (view.viewMode != RenderViewMode::ReferencePathTracer)
	{
		m_lastActionViewportId = view.viewportId;
		m_lastActionSequence = actionSequence;
		m_selected = false;
		Suspend();
		return false;
	}
	m_selected = true;
	return true;
}

void ReferencePathTracerSession::ApplyPendingTransfer(const RenderView& view) noexcept
{
	if (m_transferPending && m_transferViewportId == view.viewportId && !m_pendingCommit)
	{
		m_discardedSamples = m_committedSamples;
		m_resources.Release();
		m_identity = {};
		m_hasIdentity = false;
		m_hasOwner = false;
		m_transferViewportId = 0u;
		m_transferPending = false;
		m_lastReason = ViewportRenderProgressReason::ViewChanged;
		m_unavailableReason = ViewportRenderProgressReason::None;
		++m_executionGeneration;
	}
}

bool ReferencePathTracerSession::AdmitViewport(const RenderView& view) noexcept
{
	if (m_hasOwner && m_ownerViewportId != view.viewportId)
	{
		m_retentionAvailable = false;
		m_unavailableReason = ViewportRenderProgressReason::SessionCapacity;
		m_lastReason = m_unavailableReason;
		return false;
	}
	if (m_hasOwner && m_ownerViewportId == view.viewportId && m_unavailableReason == ViewportRenderProgressReason::SessionCapacity)
	{
		m_unavailableReason = ViewportRenderProgressReason::None;
	}

	return true;
}

bool ReferencePathTracerSession::ActivateViewport(const RenderFrame& frame, RayTracingExecutionFrontend executionFrontend) noexcept
{
	const RenderView& view = frame.View;
	const bool resumed = m_suspended && m_hasIdentity;
	UpdateIdentity(frame, executionFrontend);
	m_suspended = false;
	m_suspensionPending = false;
	if (m_unavailableReason != ViewportRenderProgressReason::None)
	{
		return false;
	}
	m_ownerViewportId = view.viewportId;
	m_hasOwner = true;
	if (resumed && m_hasIdentity && !m_resetVisible)
	{
		m_lastReason = ViewportRenderProgressReason::Resumed;
		m_lastCommitTime = std::chrono::steady_clock::now();
	}
	m_retentionAvailable = m_resources.CanRetain();

	return true;
}

void ReferencePathTracerSession::ReleaseRetainedIdentity() noexcept
{
	m_pendingCommit = {};
	m_discardedSamples = m_committedSamples;
	m_committedSamples = 0u;
	m_lastReason = ViewportRenderProgressReason::RetentionReleased;
	m_resources.Release();
	m_hasIdentity = false;
	m_hasOwner = false;
	m_retentionAvailable = false;
}
