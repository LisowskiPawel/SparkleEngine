#pragma once

#include "RHI/Public/Diagnostics/RhiGpuCrash.h"
#include "Tasks/Public/TaskDispatch.h"
#include "Tasks/Public/TaskScope.h"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class ApplicationTaskRuntime;
class GpuCrashInputs;
struct GpuCrashStartup;

class GpuCrashIncidentSink final : public RhiGpuCrashArtifactSink
{
public:
	GpuCrashIncidentSink(ApplicationTaskRuntime& runtime, const GpuCrashStartup& startup, ERhiBackendApi api);
	~GpuCrashIncidentSink() override;

	bool IsReadyPublished() const;
	void AbortPreparation() noexcept;
	std::string_view GetIncidentId() const noexcept override;

	bool Enqueue(RhiGpuCrashArtifactId id, std::span<const std::byte> bytes) noexcept override;
	void MarkIncomplete() noexcept override;
	void MarkReady() noexcept override;
	void BeginCollection(std::int64_t nativeFailure) noexcept override;
	void Finalize(bool sdkFinished, bool timedOut, std::int64_t nativeFailure) noexcept override;
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

	TaskResult Drain(TaskExecutionContext& context);
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

	TaskScope m_scope;
	TaskDispatch m_dispatch;
	std::filesystem::path m_directory;
	std::string m_identity;
	std::string m_executableHash;
	std::unique_ptr<GpuCrashInputs> m_inputs;
	bool m_readyPublished = false;
	bool m_keepSlot = false;
	std::map<std::string, FileReceipt> m_files;
	std::size_t m_writtenBytes = 0;

	mutable std::mutex m_mutex;
	std::array<Payload, MaximumPayloads> m_queue;
	std::size_t m_head = 0;
	std::size_t m_count = 0;
	std::size_t m_outstandingCount = 0;
	std::size_t m_outstandingBytes = 0;
	std::atomic_bool m_incomplete{false};
	std::atomic_bool m_writeFailed{false};
	const char* m_pendingState = nullptr;
	bool m_finalizing = false;
	bool m_sdkFinished = false;
	bool m_timedOut = false;
	std::int64_t m_nativeFailure = 0;
	bool m_hasDump = false;
};
