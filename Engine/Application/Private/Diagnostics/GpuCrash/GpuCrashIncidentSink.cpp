#include "PCH.h"
#include "Core/Public/Hash/FileHash.h"

#include "Diagnostics/GpuCrash/GpuCrashIncidentSink.h"

#include "Diagnostics/GpuCrash/GpuCrashInputs.h"
#include <nlohmann/json.hpp>
#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Hash/HashUtils.h"
#include <new>

#include <chrono>
#include <iostream>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogGpuCrashSink, "Application.GpuCrash");

GpuCrashIncidentSink::GpuCrashIncidentSink(enkiTaskScheduler* scheduler, std::stop_token cancellation, const GpuCrashStartup& startup, ERhiBackendApi api) :
    m_scheduler(scheduler),
    m_cancellation(cancellation)
{
	try
	{
		if (!m_scheduler || enkiGetTaskSchedulerConfig(m_scheduler).numTaskThreadsToCreate == 0)
		{
			throw Diagnostics::Error("GPU crash collection requires an admitted native I/O worker.");
		}
		PrepareIncident(startup, api);
		PrepareWriter();
	}
	catch (...)
	{
		RollbackPreparation();
		throw;
	}
}

void GpuCrashIncidentSink::PrepareIncident(const GpuCrashStartup& startup, ERhiBackendApi api)
{
	Hash::Sha256Digest executableHash;
	std::string error;
	const auto executablePath = Filesystem::GetExecutablePath();
	if (!Hash::TrySha256File(executablePath, executableHash, error))
	{
		throw Diagnostics::Error(error);
	}
	m_executableHash = Hash::Sha256ToHex(executableHash);
	m_identity = startup.SessionId;
	ReserveSlot();
	const auto manifestFile = m_directory / "incident.json";
	const auto manifestPath = Strings::ToNarrow(manifestFile.wstring());
	const auto quotedManifest = nlohmann::json(manifestPath).dump();
	std::cout << "SPARKLE_GPU_CRASH_INCIDENT " << quotedManifest << std::endl;
	m_inputs = std::make_unique<GpuCrashInputs>(startup, api);
	WriteManifest("Preparing");
}

void GpuCrashIncidentSink::PrepareWriter()
{
	m_writer = enkiCreatePinnedTask(m_scheduler, RunWriter, enkiGetTaskSchedulerConfig(m_scheduler).numExternalTaskThreads + 1);
	if (!m_writer)
	{
		throw std::bad_alloc();
	}
	enkiSetArgsPinnedTask(m_writer, this);
	m_completion = enkiCreateCompletionAction(m_scheduler, nullptr, CompleteWriter);
	if (!m_completion)
	{
		throw std::bad_alloc();
	}
	enkiSetParamsCompletionAction(m_completion, {nullptr, this, enkiGetCompletableFromPinnedTask(m_writer)});
}

void GpuCrashIncidentSink::RunWriter(void* argument)
{
	auto& sink = *static_cast<GpuCrashIncidentSink*>(argument);
	{
		std::lock_guard lock(sink.m_mutex);
		sink.m_writerPhase = WriterPhase::Running;
	}
	try
	{
		sink.Drain();
	}
	catch (...)
	{
		sink.MarkIncomplete();
		sink.m_bWriteFailed.store(true, std::memory_order_relaxed);
		sink.CloseWriter();
	}
}

void GpuCrashIncidentSink::RollbackPreparation() noexcept
{
	if (!m_directory.empty())
	{
		m_bKeepSlot = true;
		try
		{
			WriteManifest("Unavailable");
		}
		catch (...)
		{
		}
	}
	CloseWriter();
	WaitForWriter(std::chrono::seconds(10));
	if (m_completion)
	{
		enkiDeleteCompletionAction(m_scheduler, m_completion);
	}
	if (m_writer)
	{
		enkiDeletePinnedTask(m_scheduler, m_writer);
	}
	RemoveUnusedSlot();
}

bool GpuCrashIncidentSink::IsReadyPublished() const
{
	std::lock_guard lock(m_mutex);
	if (m_nativeFailure != 0 || m_bFinalizing)
	{
		return m_bReadyPublished;
	}

	const bool bCollectionFailed = m_bIncomplete.load(std::memory_order_relaxed) || m_bWriteFailed.load(std::memory_order_relaxed);
	if (bCollectionFailed)
	{
		throw Diagnostics::Error("GPU crash startup publication failed; this child cannot confirm diagnostic readiness.");
	}
	return m_bReadyPublished;
}

GpuCrashIncidentSink::~GpuCrashIncidentSink()
{
	if (!IsSettled())
	{
		Diagnostics::Fatal(LogGpuCrashSink, __FILE__, __LINE__, "GPU crash sink must settle after native callback admission closes, before destruction.");
	}
	if (!WaitForWriter(std::chrono::seconds(10)))
	{
		// Returning would free a task capture or a still-borrowed callback sink.
		Diagnostics::Fatal(LogGpuCrashSink, __FILE__, __LINE__, "GPU crash writer did not settle; retaining its storage until process exit.");
	}
	enkiDeleteCompletionAction(m_scheduler, m_completion);
	enkiDeletePinnedTask(m_scheduler, m_writer);
	RemoveUnusedSlot();
}

void GpuCrashIncidentSink::AbortPreparation() noexcept
{
	m_bKeepSlot = true;
	MarkIncomplete();
	Finalize(false, false, 0);
	// Startup owns this scope; any successfully enabled RHI session has sealed first.
	if (!WaitForWriter(std::chrono::seconds(10)))
	{
		Diagnostics::Fatal(LogGpuCrashSink, __FILE__, __LINE__, "GPU crash preparation rollback did not settle.");
	}
}

bool GpuCrashIncidentSink::Enqueue(RhiGpuCrashArtifactId id, std::span<const std::byte> bytes) noexcept
{
	bool bReserved = false;
	try
	{
		{
			std::lock_guard lock(m_mutex);
			const bool bWriterUnavailable = m_bFinalizing || m_bWriteFailed.load(std::memory_order_relaxed);
			const bool bPayloadRejected = bWriterUnavailable || bytes.empty();
			const bool bQueueFull = m_outstandingCount == MaximumPayloads || bytes.size() > MaximumQueuedBytes - m_outstandingBytes;

			if (bPayloadRejected || bQueueFull)
			{
				MarkIncomplete();
				return false;
			}
			++m_outstandingCount;
			m_outstandingBytes += bytes.size();
			bReserved = true;
		}

		Payload payload;
		payload.Id = id;
		payload.Bytes.assign(bytes.begin(), bytes.end());

		{
			std::lock_guard lock(m_mutex);
			m_queue[(m_head + m_count) % MaximumPayloads] = std::move(payload);
			++m_count;
			bReserved = false;
		}

		if (!NotifyWriter())
		{
			MarkIncomplete();
			return false;
		}
		return true;
	}
	catch (...)
	{
		if (bReserved)
		{
			std::lock_guard lock(m_mutex);
			--m_outstandingCount;
			m_outstandingBytes -= bytes.size();
		}
		MarkIncomplete();
		return false;
	}
}

void GpuCrashIncidentSink::MarkIncomplete() noexcept
{
	m_bIncomplete.store(true, std::memory_order_relaxed);
}

void GpuCrashIncidentSink::MarkReady() noexcept
{
	{
		std::lock_guard lock(m_mutex);
		if (m_nativeFailure == 0 && !m_bFinalizing)
		{
			m_pendingState = "Ready";
		}
	}
	if (!NotifyWriter())
	{
		MarkIncomplete();
	}
}

void GpuCrashIncidentSink::BeginCollection(std::int64_t nativeFailure) noexcept
{
	{
		std::lock_guard lock(m_mutex);
		m_bReadyPublished = false;
		if (m_bFinalizing)
		{
			return;
		}
		m_nativeFailure = nativeFailure;
		m_pendingState = "Collecting";
	}
	if (!NotifyWriter())
	{
		MarkIncomplete();
	}
}

void GpuCrashIncidentSink::Finalize(bool bSdkFinished, bool bTimedOut, std::int64_t nativeFailure) noexcept
{
	{
		std::lock_guard lock(m_mutex);
		m_bReadyPublished = false;
		if (m_bFinalizing)
		{
			return;
		}
		m_bFinalizing = true;
		m_bSdkFinished = bSdkFinished;
		m_bTimedOut = bTimedOut;
		m_nativeFailure = nativeFailure;
	}
	if (!NotifyWriter(true))
	{
		MarkIncomplete();
		CloseWriter();
	}
}

bool GpuCrashIncidentSink::IsSettled() const noexcept
{
	std::lock_guard lock(m_mutex);
	return m_bWriterClosed && m_writerPhase == WriterPhase::Idle;
}

std::string_view GpuCrashIncidentSink::GetIncidentId() const noexcept
{
	return m_identity;
}

bool GpuCrashIncidentSink::Pop(Payload& payload) noexcept
{
	std::lock_guard lock(m_mutex);
	if (m_count == 0)
	{
		return false;
	}

	payload = std::move(m_queue[m_head]);
	m_head = (m_head + 1) % MaximumPayloads;
	--m_count;
	return true;
}

void GpuCrashIncidentSink::Drain()
{
	for (std::size_t processed = 0; processed < 8 && !m_cancellation.stop_requested(); ++processed)
	{
		Payload payload;
		if (!Pop(payload))
		{
			break;
		}
		try
		{
			if (!m_bWriteFailed.load(std::memory_order_relaxed))
			{
				WritePayload(payload);
			}
		}
		catch (...)
		{
			MarkIncomplete();
			m_bWriteFailed.store(true, std::memory_order_relaxed);
		}

		const std::size_t size = payload.Bytes.size();
		std::vector<std::byte>{}.swap(payload.Bytes);
		std::lock_guard lock(m_mutex);
		--m_outstandingCount;
		m_outstandingBytes -= size;
	}

	if (m_cancellation.stop_requested())
	{
		Payload discarded;
		while (Pop(discarded))
		{
			const auto size = discarded.Bytes.size();
			std::vector<std::byte>{}.swap(discarded.Bytes);
			std::lock_guard lock(m_mutex);
			--m_outstandingCount;
			m_outstandingBytes -= size;
		}
		MarkIncomplete();
		try
		{
			WriteManifest("Partial");
		}
		catch (...)
		{
			m_bWriteFailed.store(true, std::memory_order_relaxed);
		}
		CloseWriter();
		return;
	}

	const char* pendingState = nullptr;
	bool bFinalize = false;
	bool bMore = false;
	{
		std::lock_guard lock(m_mutex);
		pendingState = std::exchange(m_pendingState, nullptr);
		if (m_bFinalizing)
		{
			pendingState = nullptr;
		}
		bFinalize = m_bFinalizing && m_outstandingCount == 0;
		bMore = m_count != 0;
	}

	if (pendingState != nullptr)
	{
		try
		{
			WriteManifest(pendingState);
		}
		catch (...)
		{
			MarkIncomplete();
			m_bWriteFailed.store(true, std::memory_order_relaxed);
		}
	}

	if (bFinalize)
	{
		try
		{
			const char* state = "Timed out";
			if (!m_bTimedOut)
			{
				const bool bDumpFinished = m_bSdkFinished && m_bHasDump;
				const bool bCollected = bDumpFinished && !m_bIncomplete.load(std::memory_order_relaxed);
				state = bCollected ? "Collected" : "Partial";
			}
			WriteManifest(state);
		}
		catch (...)
		{
			MarkIncomplete();
			CloseWriter();
			throw;
		}
		CloseWriter();
	}
	else if (bMore && !NotifyWriter())
	{
		MarkIncomplete();
	}
	return;
}

// The post action is the writer's sole dependent. Upstream permits deleting the
// action here; the native body and predecessor's last borrowed access are done.
void GpuCrashIncidentSink::CompleteWriter(void* argument, std::uint32_t) noexcept
{
	auto& sink = *static_cast<GpuCrashIncidentSink*>(argument);
	std::lock_guard lock(sink.m_mutex);
	if (std::exchange(sink.m_bWriterRerun, false))
	{
		sink.m_writerPhase = WriterPhase::Queued;
		enkiAddPinnedTask(sink.m_scheduler, sink.m_writer);
	}
	else
	{
		sink.m_writerPhase = WriterPhase::Idle;
		sink.m_writerSettled.notify_all();
	}
}

bool GpuCrashIncidentSink::NotifyWriter(bool bCleanup) noexcept
{
	std::lock_guard lock(m_mutex);
	if (m_bWriterClosed || (!bCleanup && m_cancellation.stop_requested()))
	{
		return false;
	}
	if (m_writerPhase == WriterPhase::Running)
	{
		m_bWriterRerun = true;
	}
	else if (m_writerPhase == WriterPhase::Idle)
	{
		m_writerPhase = WriterPhase::Queued;
		enkiAddPinnedTask(m_scheduler, m_writer);
	}
	return true;
}

void GpuCrashIncidentSink::CloseWriter() noexcept
{
	std::lock_guard lock(m_mutex);
	m_bWriterClosed = true;
	if (m_writerPhase == WriterPhase::Idle)
	{
		m_writerSettled.notify_all();
	}
	if (m_cancellation.stop_requested() || m_bWriteFailed.load(std::memory_order_relaxed))
	{
		m_bWriterRerun = false;
	}
}

bool GpuCrashIncidentSink::WaitForWriter(std::chrono::milliseconds timeout) const noexcept
{
	std::unique_lock lock(m_mutex);
	return m_writerSettled.wait_for(lock, timeout, [this] { return m_bWriterClosed && m_writerPhase == WriterPhase::Idle; });
}
