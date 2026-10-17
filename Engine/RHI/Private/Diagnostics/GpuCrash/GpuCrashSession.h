#pragma once

#include "Diagnostics/RhiGpuCrash.h"

#include <GFSDK_Aftermath_GpuCrashDump.h>
#include <GFSDK_Aftermath_GpuCrashDumpDecoding.h>
#include <Windows.h>
#include <array>
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string_view>

class GpuCrashSession final
{
public:
	explicit GpuCrashSession(ERhiBackendApi api, const RhiGpuCrashLaunch& launch);
	static GpuCrashSession* Current() noexcept;

	FARPROC RequireExport(const char* name) const;

	void Ready() noexcept;
	void Incomplete() noexcept;
	void Enqueue(RhiGpuCrashArtifactId id, std::span<const std::byte> bytes) noexcept;
	void ShaderIdentity(std::span<const std::byte> code, std::uint64_t nativeHash, bool hashAvailable, const GFSDK_Aftermath_ShaderDebugName* debugName) noexcept;
	bool BeginLoss(std::int64_t nativeFailure) noexcept;
	void FinishLoss(std::int64_t nativeFailure) noexcept;
	void Shutdown() noexcept;

	const void* Marker(std::string_view label, std::uint32_t parent, char operation, std::uint32_t& id) noexcept;
	std::string_view MarkerText(const void* token) const noexcept;

private:
	class SinkBorrow final
	{
	public:
		explicit SinkBorrow(GpuCrashSession& session) noexcept;
		~SinkBorrow();
		SinkBorrow(const SinkBorrow&) = delete;
		SinkBorrow& operator=(const SinkBorrow&) = delete;
		RhiGpuCrashArtifactSink* Get() const noexcept;

	private:
		GpuCrashSession& m_session;
		RhiGpuCrashArtifactSink* m_sink = nullptr;
	};

	struct MarkerEntry final
	{
		std::uint32_t Parent = 0;
		char Operation = 0;
		std::array<char, 96> Label{};
		std::array<char, 128> Text{};
	};

	static void GFSDK_AFTERMATH_CALL DumpCallback(const void* data, std::uint32_t size, void* user) noexcept;
	static void GFSDK_AFTERMATH_CALL DebugCallback(const void* data, std::uint32_t size, void* user) noexcept;
	static void GFSDK_AFTERMATH_CALL DescriptionCallback(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription add, void* user) noexcept;
	static void GFSDK_AFTERMATH_CALL MarkerCallback(const void* marker, std::uint32_t size, void* user, PFN_GFSDK_Aftermath_ResolveMarker resolve) noexcept;

	void InitializeLibrary(ERhiBackendApi api, const std::filesystem::path& libraryPath);
	void Settle(bool loss, std::int64_t nativeFailure) noexcept;
	bool SealSink(std::chrono::steady_clock::time_point deadline) noexcept;

	static constexpr std::size_t MaximumMarkers = 65'536;

	HMODULE m_library = nullptr;
	PFN_GFSDK_Aftermath_EnableGpuCrashDumps m_enable = nullptr;
	PFN_GFSDK_Aftermath_DisableGpuCrashDumps m_disable = nullptr;
	PFN_GFSDK_Aftermath_GetCrashDumpStatus m_status = nullptr;
	PFN_GFSDK_Aftermath_GetShaderDebugInfoIdentifier m_debugIdentifier = nullptr;

	mutable std::mutex m_sinkMutex;
	RhiGpuCrashArtifactSink* m_sink = nullptr;
	std::size_t m_borrows = 0;
	bool m_accepting = true;
	std::atomic<bool> m_lossClaimed{false};
	std::atomic<bool> m_incomplete{false};
	std::atomic<std::uint64_t> m_shaderSequence{0};
	std::array<char, 37> m_incidentId{};

	mutable std::mutex m_markerMutex;
	std::unique_ptr<MarkerEntry[]> m_markers;
	std::size_t m_markerCount = 0;
	MarkerEntry m_unknown;
	std::chrono::steady_clock::time_point m_lossDeadline;
};

// Backend implementation hook; no native types cross the diagnostics target.
void PrepareD3D12GpuCrash();

class GpuCrashScopes final
{
public:
	const void* Begin(std::string_view label) noexcept;
	const void* End() noexcept;
	const void* Insert(std::string_view label) noexcept;
	void Reset() noexcept;

private:
	const void* Record(std::string_view label, char operation) noexcept;
	std::array<std::uint32_t, 32> m_parents{};
	std::size_t m_depth = 0;
	std::size_t m_overflowDepth = 0;
};
