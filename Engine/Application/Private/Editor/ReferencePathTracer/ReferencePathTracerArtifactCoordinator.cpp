#include "PCH.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactCoordinator.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Identifiers/Uuid.h"
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactWriter.h"
#include "Renderer.h"

#include <TaskScheduler_c.h>
#include "Core/Public/Diagnostics/Verify.h"
#include <new>
#include <optional>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_referenceArtifactLogger, "Application.ReferenceArtifact");

struct ReferencePathTracerArtifactCoordinator::Write final
{
	ReferencePathTracerArtifactWriteRequest Request;
	std::stop_token Stop;
	ReferencePathTracerArtifactWriteResult Result;
	enkiPinnedTask* Task = nullptr;
};

ReferencePathTracerArtifactCoordinator::ReferencePathTracerArtifactCoordinator(enkiTaskScheduler* io, std::stop_token stop) noexcept :
    m_io(io),
    m_stop(stop)
{
}

ReferencePathTracerArtifactCoordinator::~ReferencePathTracerArtifactCoordinator()
{
	if (m_write)
	{
		Diagnostics::Fatal(g_referenceArtifactLogger, __FILE__, __LINE__, "Reference artifact work must be consumed or explicitly shut down before owner destruction.");
	}
}

void ReferencePathTracerArtifactCoordinator::Shutdown() noexcept
{
	if (m_write)
	{
		if (m_write->Task)
		{
			enkiWaitForPinnedTask(m_io, m_write->Task);
			enkiDeletePinnedTask(m_io, m_write->Task);
		}
		m_write.reset();
	}
	m_meanCapture.Discard();
	m_moment2Capture.Discard();
	m_mean.reset();
	m_moment2.reset();
	m_pendingKind.reset();
}

void ReferencePathTracerArtifactCoordinator::WriteArtifact(void* argument) noexcept
{
	auto& write = *static_cast<Write*>(argument);
	write.Result = ReferencePathTracerArtifactWriter::Write(std::move(write.Request), write.Stop);
}

void ReferencePathTracerArtifactCoordinator::Request(
    ReferencePathTracerArtifactKind kind,
    Renderer& renderer,
    const ViewportRenderProducts& products,
    const std::filesystem::path& outputRoot,
    std::uint64_t maximumOutputBytes)
{
	if (m_stop.stop_requested())
	{
		Fail("Reference artifact owner is closed.");
		return;
	}
	if (!IsSettled())
	{
		m_lastResult = ReferencePathTracerArtifactWriteResult{.ErrorMessage = "A Reference artifact operation is already in progress."};
		return;
	}
	if (products.GetProgress().State == ViewportRenderProgressState::Unavailable)
	{
		Fail("The current viewport does not own an available Reference Path Tracer prefix.");
		return;
	}
	m_maximumOutputBytes = maximumOutputBytes;
	m_outputRoot = outputRoot.empty() ? DefaultOutputRoot() : outputRoot;
	if (kind == ReferencePathTracerArtifactKind::Complete)
	{
		Begin(kind, renderer, products, m_outputRoot);
		return;
	}

	const RenderProduct* mean = products.FindProduct(RenderOutputFlags::Radiance);
	if (mean == nullptr || mean->SamplePrefix.SampleCount == 0u)
	{
		Fail("No committed radiance sample prefix is available for publication.");
		return;
	}
	m_pendingKind = kind;
	m_pendingPrefix = mean->SamplePrefix;
}

void ReferencePathTracerArtifactCoordinator::Begin(ReferencePathTracerArtifactKind kind, Renderer& renderer, const ViewportRenderProducts& products, const std::filesystem::path& outputRoot)
{
	const RenderProduct* mean = products.FindProduct(RenderOutputFlags::Radiance);
	const ViewportRenderProgress& progress = products.GetProgress();
	if (mean == nullptr || mean->SamplePrefix.SampleCount == 0u)
	{
		Fail("No committed radiance sample prefix is available for publication.");
		return;
	}
	if (kind == ReferencePathTracerArtifactKind::Complete && progress.State != ViewportRenderProgressState::Complete)
	{
		Fail("The requested complete prefix has not reached its target.");
		return;
	}

	m_kind = kind;
	m_outputRoot = outputRoot;
	const std::string digest = Hash::Sha256ToHex(mean->SamplePrefix.RenderIdentitySha256);
	const char* prefixLabel = kind == ReferencePathTracerArtifactKind::Checkpoint ? "Checkpoint" : "PartialPrefix";
	const char* label = kind == ReferencePathTracerArtifactKind::Complete ? "Complete" : prefixLabel;
	const std::string publicationId = Identifiers::CreateUuidV4String();
	const std::string directoryName = publicationId + "-" + label + "-" + std::to_string(mean->SamplePrefix.SampleCount);
	m_publicationDirectory = m_outputRoot / digest / directoryName;
	m_lastResult.reset();
	m_mean.reset();
	m_moment2.reset();
	m_capturePrefix = mean->SamplePrefix;
	const ViewportCaptureAdmissionStatus meanAdmission = m_meanCapture.Request(renderer, ViewportCaptureRequest{.Output = RenderOutputFlags::Radiance});
	if (meanAdmission != ViewportCaptureAdmissionStatus::Accepted)
	{
		Fail(
		    meanAdmission == ViewportCaptureAdmissionStatus::Full ? "Reference radiance readback rejected: render request capacity is full."
		                                                          : "Reference radiance readback rejected: render owner is closed.");

		return;
	}
	if (kind == ReferencePathTracerArtifactKind::Checkpoint)
	{
		const ViewportCaptureAdmissionStatus moment2Admission = m_moment2Capture.Request(renderer, ViewportCaptureRequest{.Output = RenderOutputFlags::RadianceSecondMoment});
		if (moment2Admission != ViewportCaptureAdmissionStatus::Accepted)
		{
			Fail(
			    moment2Admission == ViewportCaptureAdmissionStatus::Full ? "Checkpoint M2 readback rejected: render request capacity is full."
			                                                             : "Checkpoint M2 readback rejected: render owner is closed.");
		}
	}
}

void ReferencePathTracerArtifactCoordinator::CollectReadback(Renderer& renderer, ViewportCaptureSlot& capture, std::optional<ViewportCaptureReadback>& destination)
{
	capture.Update(renderer);
	if (!capture.HasReadback())
	{
		return;
	}
	ViewportCaptureReadback readback = capture.TakeReadback();
	if (!readback.Result)
	{
		Fail(std::move(readback.Result.FailureReason));
		return;
	}
	if (readback.Result.SamplePrefix != m_capturePrefix)
	{
		Fail("The radiance readback no longer belongs to the requested immutable sample prefix.");
		return;
	}
	destination = std::move(readback);
}

void ReferencePathTracerArtifactCoordinator::PublishIfReady()
{
	if (!m_mean || (m_kind == ReferencePathTracerArtifactKind::Checkpoint && !m_moment2))
	{
		return;
	}
	if (m_moment2)
	{
		const auto& meanPrefix = m_mean->Result.SamplePrefix;
		const auto& moment2Prefix = m_moment2->Result.SamplePrefix;
		const bool bSamePrefix = meanPrefix.RenderIdentitySha256 == moment2Prefix.RenderIdentitySha256 && meanPrefix.SampleCount == moment2Prefix.SampleCount;
		const bool bSameExtent = m_mean->Width == m_moment2->Width && m_mean->Height == m_moment2->Height;
		if (!bSamePrefix || !bSameExtent)
		{
			Fail("Checkpoint planes do not describe the same immutable session prefix.");
			return;
		}
	}

	ReferencePathTracerArtifactWriteRequest request{
	    .Kind = m_kind,
	    .AllowedRoot = m_outputRoot,
	    .PublicationDirectory = m_publicationDirectory,
	    .MaximumOutputBytes = m_maximumOutputBytes,
	    .Mean = std::move(*m_mean),
	    .Moment2 = std::move(m_moment2)};

	m_mean.reset();
	m_moment2.reset();
	StartPublication(std::move(request));
}

void ReferencePathTracerArtifactCoordinator::StartPublication(ReferencePathTracerArtifactWriteRequest request)
{
	if (m_write || m_stop.stop_requested())
	{
		Fail("Reference artifact publication owner is occupied or closed.");
		return;
	}
	try
	{
		auto write = std::make_unique<Write>(std::move(request), m_stop);
		PrepareNativeWork(*write);
		m_write = std::move(write);
		if (m_io)
		{
			enkiAddPinnedTask(m_io, m_write->Task);
		}
		else
		{
			WriteArtifact(m_write.get());
		}
	}
	catch (const std::exception& error)
	{
		Fail(error.what());
	}
}

void ReferencePathTracerArtifactCoordinator::Update(Renderer& renderer, const ViewportRenderProducts& products)
{
	CollectReadback(renderer, m_meanCapture, m_mean);
	CollectReadback(renderer, m_moment2Capture, m_moment2);
	PublishIfReady();
	BeginPendingCapture(renderer, products);
	CollectPublicationResult();
}

void ReferencePathTracerArtifactCoordinator::BeginPendingCapture(Renderer& renderer, const ViewportRenderProducts& products)
{
	if (!m_pendingKind)
	{
		return;
	}

	const RenderProduct* mean = products.FindProduct(RenderOutputFlags::Radiance);
	const bool bSameIdentity = mean != nullptr && mean->SamplePrefix.RenderIdentitySha256 == m_pendingPrefix.RenderIdentitySha256;
	const bool bSameTarget = bSameIdentity && mean->SamplePrefix.TargetSampleCount == m_pendingPrefix.TargetSampleCount;
	if (!bSameTarget)
	{
		Fail("The output intent was cancelled because the session identity changed before its prefix settled.");
		return;
	}

	const ViewportRenderProgressState state = products.GetProgress().State;
	if (state == ViewportRenderProgressState::Unavailable)
	{
		Fail("The output intent could not settle because the session became unavailable.");
		return;
	}
	if (state == ViewportRenderProgressState::Paused || state == ViewportRenderProgressState::Complete)
	{
		const ReferencePathTracerArtifactKind kind = *m_pendingKind;
		m_pendingKind.reset();
		Begin(kind, renderer, products, m_outputRoot);
	}
}

void ReferencePathTracerArtifactCoordinator::CollectPublicationResult()
{
	if (!m_write)
	{
		return;
	}
	if (m_write->Task && !enkiIsPinnedTaskComplete(m_io, m_write->Task))
	{
		return;
	}
	auto result = std::move(m_write->Result);
	if (m_write->Task)
	{
		enkiDeletePinnedTask(m_io, m_write->Task);
	}
	m_write.reset();
	m_lastResult = std::move(result);
}

bool ReferencePathTracerArtifactCoordinator::IsSettled() const noexcept
{
	if (m_pendingKind)
	{
		return false;
	}
	const bool bReadbacksSettled = m_meanCapture.IsSettled() && m_moment2Capture.IsSettled();
	return bReadbacksSettled && !m_write;
}

std::optional<ReferencePathTracerArtifactWriteResult> ReferencePathTracerArtifactCoordinator::TakeResult() noexcept
{
	return std::exchange(m_lastResult, std::nullopt);
}

void ReferencePathTracerArtifactCoordinator::Fail(std::string message)
{
	m_meanCapture.Discard();
	m_moment2Capture.Discard();
	m_mean.reset();
	m_moment2.reset();
	m_pendingKind.reset();
	m_capturePrefix = {};
	m_lastResult = ReferencePathTracerArtifactWriteResult{.bSucceeded = false, .PublicationDirectory = m_publicationDirectory, .ErrorMessage = std::move(message)};
}

std::filesystem::path ReferencePathTracerArtifactCoordinator::DefaultOutputRoot()
{
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	return userState.CapturesRoot / "ReferencePathTracer";
}

void ReferencePathTracerArtifactCoordinator::PrepareNativeWork(Write& write)
{
	if (!m_io)
	{
		return;
	}

	const auto config = enkiGetTaskSchedulerConfig(m_io);
	const auto worker = config.numExternalTaskThreads + 1 + 1 % config.numTaskThreadsToCreate;
	write.Task = enkiCreatePinnedTask(m_io, WriteArtifact, worker);
	if (!write.Task)
	{
		throw std::bad_alloc();
	}
	enkiSetArgsPinnedTask(write.Task, &write);
}
