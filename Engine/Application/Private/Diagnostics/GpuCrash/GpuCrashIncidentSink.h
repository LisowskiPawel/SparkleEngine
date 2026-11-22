#pragma once

#include "RHI/Public/Diagnostics/RhiGpuCrash.h"
#include <TaskScheduler_c.h>
#include <stop_token>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class GpuCrashInputs;
struct GpuCrashStartup;

class GpuCrashIncidentSink final : public RhiGpuCrashArtifactSink
{
public:
	GpuCrashIncidentSink(enkiTaskScheduler* scheduler, std::stop_token cancellation, const GpuCrashStartup& startup, ERhiBackendApi api);
	~GpuCrashIncidentSink() override;

	bool IsReadyPublished() const;
	void AbortPreparation() noexcept;
	std::string_view GetIncidentId() const noexcept override;

	bool Enqueue(RhiGpuCrashArtifactId id, std::span<const std::byte> bytes) noexcept override;
	void MarkIncomplete() noexcept override;
	void MarkReady() noexcept override;
	void BeginCollection(std::int64_t nativeFailure) noexcept override;
	void Finalize(bool bSdkFinished, bool bTimedOut, std::int64_t nativeFailure) noexcept override;
	bool IsSettled() const noexcept override;

private:
	struct Payload final
	{
		RhiGpuCrashArtifactId Id;
		std::vector<std::byte> Bytes;
	};

	struct FileReceipt final
	{
		std::string Sha256;
		std::size_t Bytes = 0;
	};

	void PrepareIncident(const GpuCrashStartup& startup, ERhiBackendApi api);
	void PrepareWriter();
	void RollbackPreparation() noexcept;
	static void RunWriter(void* argument);
	void Drain();
	bool NotifyWriter(bool bCleanup = false) noexcept;
	void CloseWriter() noexcept;
	bool WaitForWriter(std::chrono::milliseconds timeout) const noexcept;
	static void CompleteWriter(void* argument, std::uint32_t thread) noexcept;
	bool Pop(Payload& payload) noexcept;

	void WritePayload(const Payload& payload);
	std::string BuildManifest(std::string_view state) const;
	void WriteManifest(std::string_view state);
	void RemoveUnusedSlot() noexcept;
	void ReserveSlot();
	static void WriteDurableFile(const std::filesystem::path& path, std::span<const std::byte> bytes);
	static std::string BuildFileName(RhiGpuCrashArtifactId id);

	static constexpr std::size_t MaximumPayloads = 256;
	static constexpr std::size_t MaximumQueuedBytes = 256u * 1024u * 1024u;
	static constexpr std::size_t MaximumIncidentBytes = 1024u * 1024u * 1024u;
	static constexpr std::size_t MaximumFiles = 65'536;
	static constexpr std::size_t MetadataAllowance = 64u * 1024u * 1024u;

	enkiTaskScheduler* m_scheduler;
	std::stop_token m_cancellation;
	enkiPinnedTask* m_writer = nullptr;
	enkiCompletionAction* m_completion = nullptr;
	enum class WriterPhase : std::uint8_t
	{
		Idle,
		Queued,
		Running
	};
	WriterPhase m_writerPhase = WriterPhase::Idle;
	bool m_bWriterRerun = false;
	bool m_bWriterClosed = false;
	std::filesystem::path m_directory;
	std::string m_identity;
	std::string m_executableHash;
	std::unique_ptr<GpuCrashInputs> m_inputs;
	bool m_bReadyPublished = false;
	bool m_bKeepSlot = false;
	std::map<std::string, FileReceipt> m_files;
	std::size_t m_writtenBytes = 0;

	mutable std::mutex m_mutex;
	mutable std::condition_variable m_writerSettled;
	std::array<Payload, MaximumPayloads> m_queue;
	std::size_t m_head = 0;
	std::size_t m_count = 0;
	std::size_t m_outstandingCount = 0;
	std::size_t m_outstandingBytes = 0;
	std::atomic_bool m_bIncomplete{false};
	std::atomic_bool m_bWriteFailed{false};
	const char* m_pendingState = nullptr;
	bool m_bFinalizing = false;
	bool m_bSdkFinished = false;
	bool m_bTimedOut = false;
	std::int64_t m_nativeFailure = 0;
	bool m_bHasDump = false;
};
