#include "PCH.h"

#include "Diagnostics/GpuCrash/GpuCrashIncidentSink.h"

#include "Concurrency/ApplicationTaskRuntime.h"
#include "Diagnostics/GpuCrash/GpuCrashInputs.h"
#include "Core/Public/Json/JsonWriter.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Tasks/Public/TaskExecutor.h"

#include <chrono>
#include <iostream>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogGpuCrashSink, "Application.GpuCrash");

GpuCrashIncidentSink::GpuCrashIncidentSink(ApplicationTaskRuntime& runtime, const GpuCrashStartup& startup, ERhiBackendApi api) :
    m_scope(TaskScopeDesc{TaskScopeKind::Application, "GPU crash artifacts"}, &runtime.GetApplicationScope())
{
	try
	{
		Hash::Sha256Digest executableHash;
		std::string error;
		if (!Hash::TrySha256File(Filesystem::GetExecutablePath(), executableHash, error))
		{
			throw Diagnostics::Error(error);
		}
		m_executableHash = Hash::Sha256ToHex(executableHash);
		m_identity = startup.SessionId;
		ReserveSlot();
		const auto manifestPath = Strings::ToNarrow((m_directory / "incident.json").wstring());
		const auto quotedManifest = Json::QuoteString(manifestPath);
		std::cout << "SPARKLE_GPU_CRASH_INCIDENT " << quotedManifest << std::endl;
		m_inputs = std::make_unique<GpuCrashInputs>(startup, api);
		WriteManifest("Preparing");

		m_dispatch = runtime.GetExecutor().PrepareDispatch(
		    m_scope,
		    TaskDesc{TaskName("Write GPU crash artifacts"), TaskLane::BlockingIo},
		    [this](TaskExecutionContext& context) { return Drain(context); });
	}
	catch (...)
	{
		if (!m_directory.empty())
		{
			m_keepSlot = true;
			try
			{
				WriteManifest("Unavailable");
			}
			catch (...)
			{
			}
		}
		m_dispatch.Close();
		m_scope.Cancel();
		m_scope.JoinFor(std::chrono::seconds(10));
		RemoveUnusedSlot();
		throw;
	}
}

bool GpuCrashIncidentSink::IsReadyPublished() const
{
	std::lock_guard lock(m_mutex);
	if (m_nativeFailure == 0 && !m_finalizing && (m_incomplete.load(std::memory_order_relaxed) || m_writeFailed.load(std::memory_order_relaxed)))
	{
		throw Diagnostics::Error("GPU crash startup publication failed; this child cannot confirm diagnostic readiness.");
	}
	return m_readyPublished;
}

GpuCrashIncidentSink::~GpuCrashIncidentSink()
{
	if (!IsSettled())
	{
		Diagnostics::Fatal(LogGpuCrashSink, __FILE__, __LINE__, "GPU crash sink must settle after native callback admission closes, before destruction.");
	}
	if (!m_scope.JoinFor(std::chrono::seconds(10)))
	{
		// Returning would free a task capture or a still-borrowed callback sink.
		Diagnostics::Fatal(LogGpuCrashSink, __FILE__, __LINE__, "GPU crash writer did not settle; retaining its storage until process exit.");
	}
	RemoveUnusedSlot();
}

void GpuCrashIncidentSink::AbortPreparation() noexcept
{
	m_keepSlot = true;
	MarkIncomplete();
	Finalize(false, false, 0);
	// Startup owns this scope; any successfully enabled RHI session has sealed first.
	if (!m_scope.JoinFor(std::chrono::seconds(10)))
	{
		Diagnostics::Fatal(LogGpuCrashSink, __FILE__, __LINE__, "GPU crash preparation rollback did not settle.");
	}
}

bool GpuCrashIncidentSink::Enqueue(RhiGpuCrashArtifactId id, std::span<const std::byte> bytes) noexcept
{
	bool reserved = false;
	try
	{
		{
			std::lock_guard lock(m_mutex);
			if (m_finalizing || m_writeFailed.load(std::memory_order_relaxed) || bytes.empty() || m_outstandingCount == MaximumPayloads || bytes.size() > MaximumQueuedBytes - m_outstandingBytes)
			{
				MarkIncomplete();
				return false;
			}
			++m_outstandingCount;
			m_outstandingBytes += bytes.size();
			reserved = true;
		}

		Payload payload;
		payload.Id = id;
		payload.Bytes.assign(bytes.begin(), bytes.end());

		{
			std::lock_guard lock(m_mutex);
			m_queue[(m_head + m_count) % MaximumPayloads] = std::move(payload);
			++m_count;
			reserved = false;
		}

		if (!m_dispatch.Notify())
		{
			MarkIncomplete();
			return false;
		}
		return true;
	}
	catch (...)
	{
		if (reserved)
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
	m_incomplete.store(true, std::memory_order_relaxed);
}

void GpuCrashIncidentSink::MarkReady() noexcept
{
	{
		std::lock_guard lock(m_mutex);
		if (m_nativeFailure == 0 && !m_finalizing)
		{
			m_pendingState = "Ready";
		}
	}
	if (!m_dispatch.Notify())
	{
		MarkIncomplete();
	}
}

void GpuCrashIncidentSink::BeginCollection(std::int64_t nativeFailure) noexcept
{
	{
		std::lock_guard lock(m_mutex);
		m_readyPublished = false;
		if (m_finalizing)
		{
			return;
		}
		m_nativeFailure = nativeFailure;
		m_pendingState = "Collecting";
	}
	if (!m_dispatch.Notify())
	{
		MarkIncomplete();
	}
}

void GpuCrashIncidentSink::Finalize(bool sdkFinished, bool timedOut, std::int64_t nativeFailure) noexcept
{
	{
		std::lock_guard lock(m_mutex);
		m_readyPublished = false;
		if (m_finalizing)
		{
			return;
		}
		m_finalizing = true;
		m_sdkFinished = sdkFinished;
		m_timedOut = timedOut;
		m_nativeFailure = nativeFailure;
	}
	if (!m_dispatch.Notify())
	{
		MarkIncomplete();
		m_dispatch.Close();
	}
}

bool GpuCrashIncidentSink::IsSettled() const noexcept
{
	return m_dispatch.GetExecution().IsSettled();
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

TaskResult GpuCrashIncidentSink::Drain(TaskExecutionContext& context)
{
	for (std::size_t processed = 0; processed < 8 && !context.IsCancellationRequested(); ++processed)
	{
		Payload payload;
		if (!Pop(payload))
		{
			break;
		}
		try
		{
			if (!m_writeFailed.load(std::memory_order_relaxed))
			{
				WritePayload(payload);
			}
		}
		catch (...)
		{
			MarkIncomplete();
			m_writeFailed.store(true, std::memory_order_relaxed);
		}

		const std::size_t size = payload.Bytes.size();
		std::vector<std::byte>{}.swap(payload.Bytes);
		std::lock_guard lock(m_mutex);
		--m_outstandingCount;
		m_outstandingBytes -= size;
	}

	if (context.IsCancellationRequested())
	{
		MarkIncomplete();
		try
		{
			WriteManifest("Partial");
		}
		catch (...)
		{
			m_writeFailed.store(true, std::memory_order_relaxed);
		}
		m_dispatch.Close();
		return TaskResult::Cancelled("GPU crash artifact publication was cancelled.");
	}

	const char* pendingState = nullptr;
	bool finalize = false;
	bool more = false;
	{
		std::lock_guard lock(m_mutex);
		pendingState = std::exchange(m_pendingState, nullptr);
		if (m_finalizing)
		{
			pendingState = nullptr;
		}
		finalize = m_finalizing && m_outstandingCount == 0;
		more = m_count != 0;
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
			m_writeFailed.store(true, std::memory_order_relaxed);
		}
	}

	if (finalize)
	{
		try
		{
			const char* state = m_timedOut ? "Timed out" : m_sdkFinished && m_hasDump && !m_incomplete.load(std::memory_order_relaxed) ? "Collected" : "Partial";
			WriteManifest(state);
		}
		catch (...)
		{
			MarkIncomplete();
			m_dispatch.Close();
			throw;
		}
		m_dispatch.Close();
	}
	else if (more && !m_dispatch.Notify())
	{
		MarkIncomplete();
	}
	return TaskResult::Success();
}
