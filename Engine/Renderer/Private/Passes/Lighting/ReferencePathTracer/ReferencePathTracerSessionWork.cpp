#include "PCH.h"

#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerSession.h"

#include "RHI/Public/Device/RenderDeviceServices.h"

#include <algorithm>

void ReferencePathTracerSession::CompletePendingCommit() noexcept
{
	if (!m_pendingCommit || !m_deviceServices.IsSubmissionComplete(m_pendingCommit.Submission))
	{
		return;
	}
	if (m_pendingCommit.ExecutionGeneration == m_executionGeneration)
	{
		m_committedSamples = m_pendingCommit.Prefix;
		const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		const double seconds = std::chrono::duration<double>(now - m_lastCommitTime).count();
		if (seconds > 0.0)
		{
			m_samplesPerSecond = static_cast<double>(m_pixelCount) / seconds;
		}
		m_lastCommitTime = now;
	}
	m_pendingCommit = {};
	if (m_pausePending)
	{
		m_pausePending = false;
		m_paused = true;
	}
}

void ReferencePathTracerSession::PrepareWork(RenderViewportExtent extent) noexcept
{
	m_work = ReferencePathTracerWork{
	    .SessionSeed = 0u,
	    .ReplicateId = 0u,
	    .SampleOrdinal = m_committedSamples,
	    .FinitePathDiagnosticSurfaceVertices = 0u,
	    .FirstRow = m_nextRow,
	    .RowCount = (std::min) (WorkRowsPerDispatch, extent.Height - m_nextRow),
	    .PriorSampleCount = m_committedSamples,
	    .WorkFlags = ReferencePathTracerWork::WorkFlagTrace | (m_clearDisplay ? ReferencePathTracerWork::WorkFlagClearDisplay : 0u)};

	m_preparedRowCount = m_work.RowCount;
	if (m_nextRow + m_preparedRowCount == extent.Height)
	{
		m_work.WorkFlags |= ReferencePathTracerWork::WorkFlagCommitPrefix;
	}
	m_workPrepared = true;
}

void ReferencePathTracerSession::OnFrameSubmitted(RhiSubmissionToken token) noexcept
{
	if (m_selected && m_resources.IsAllocated())
	{
		m_resources.RecordSubmission();
	}
	if (m_workPrepared)
	{
		if ((m_work.WorkFlags & ReferencePathTracerWork::WorkFlagCommitPrefix) != 0u)
		{
			m_pendingCommit = PendingCommit{.Submission = token, .ExecutionGeneration = m_executionGeneration, .Prefix = m_committedSamples + 1u};
			m_nextRow = 0u;
		}
		else
		{
			m_nextRow += m_preparedRowCount;
		}
	}
	m_workPrepared = false;
	m_clearDisplay = false;
	m_resetVisible = false;
	m_work = {};
}

ViewportRenderProgress ReferencePathTracerSession::GetProgress() const noexcept
{
	ViewportRenderProgressState state = ViewportRenderProgressState::Accumulating;
	if (m_unavailableReason != ViewportRenderProgressReason::None)
	{
		state = ViewportRenderProgressState::Unavailable;
	}
	else if (m_paused)
	{
		state = ViewportRenderProgressState::Paused;
	}
	else if (m_resetVisible)
	{
		state = ViewportRenderProgressState::Resetting;
	}
	else if (m_committedSamples >= TargetSampleCount)
	{
		state = ViewportRenderProgressState::Complete;
	}

	const double remainingSamples = static_cast<double>(TargetSampleCount - (std::min) (m_committedSamples, TargetSampleCount));
	const double estimatedSeconds = m_samplesPerSecond > 0.0 && m_pixelCount > 0u ? (remainingSamples * static_cast<double>(m_pixelCount)) / m_samplesPerSecond : 0.0;

	return ViewportRenderProgress{
	    .State = state,
	    .Reason = m_unavailableReason != ViewportRenderProgressReason::None ? m_unavailableReason : m_lastReason,
	    .CompletedSamples = m_committedSamples,
	    .TargetSamples = TargetSampleCount,
	    .DiscardedSamples = m_discardedSamples,
	    .OwnerViewportId = m_ownerViewportId,
	    .SamplesPerSecond = m_samplesPerSecond,
	    .EstimatedSecondsRemaining = estimatedSeconds,
	    .RetentionAvailable = m_retentionAvailable};
}

RenderProductSamplePrefix ReferencePathTracerSession::GetRadianceSamplePrefix() const noexcept
{
	return CanPublishRadiance() ? RenderProductSamplePrefix{.RenderIdentitySha256 = m_identitySha256, .SampleCount = m_committedSamples, .TargetSampleCount = TargetSampleCount}
	                            : RenderProductSamplePrefix{};
}

bool ReferencePathTracerSession::CanPublishRadiance() const noexcept
{
	return m_committedSamples > 0u && !m_pendingCommit && (m_work.WorkFlags & ReferencePathTracerWork::WorkFlagCommitPrefix) == 0u;
}

void ReferencePathTracerSession::PrepareFrameWork(RenderViewportExtent extent) noexcept
{
	m_work = {};
	if (m_clearDisplay)
	{
		m_work.WorkFlags = ReferencePathTracerWork::WorkFlagClearDisplay;
	}
	if (!m_paused && !m_pausePending && !m_pendingCommit && m_committedSamples < TargetSampleCount)
	{
		PrepareWork(extent);
	}
}
