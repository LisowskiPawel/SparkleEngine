#pragma once

#include <cstdint>

enum class RhiGpuCrashMode : std::uint8_t
{
	Off,
	Aftermath
};

#if SPARKLE_WITH_GPU_CRASH_DEBUGGING

  #include "../Core/RhiBackendApi.h"
  #include "../RHIAPI.h"

  #include <cstddef>
  #include <cstdint>
  #include <filesystem>
  #include <span>
  #include <string_view>

enum class RhiGpuCrashArtifactKind : std::uint8_t
{
	Dump,
	ShaderDebugInfo,
	ShaderIdentity,
	NativeDiagnostics
};

struct RhiGpuCrashArtifactId final
{
	RhiGpuCrashArtifactKind Kind = RhiGpuCrashArtifactKind::Dump;
	std::uint64_t Low = 0;
	std::uint64_t High = 0;
};

class SPARKLE_RHI_API RhiGpuCrashArtifactSink
{
public:
	virtual ~RhiGpuCrashArtifactSink() = default;
	virtual std::string_view GetIncidentId() const noexcept = 0;
	virtual bool Enqueue(RhiGpuCrashArtifactId id, std::span<const std::byte> bytes) noexcept = 0;
	virtual void MarkIncomplete() noexcept = 0;
	virtual void MarkReady() noexcept = 0;
	virtual void BeginCollection(std::int64_t nativeFailure) noexcept = 0;
	virtual void Finalize(bool sdkFinished, bool timedOut, std::int64_t nativeFailure) noexcept = 0;
	virtual bool IsSettled() const noexcept = 0;
};

struct RhiGpuCrashLaunch final
{
	RhiGpuCrashMode Mode = RhiGpuCrashMode::Off;
	std::filesystem::path LibraryPath;
	RhiGpuCrashArtifactSink* Sink = nullptr;
};

class GpuCrashSession;

class SPARKLE_RHI_API RhiGpuCrash final
{
public:
	RhiGpuCrash(ERhiBackendApi api, const RhiGpuCrashLaunch& launch);
	~RhiGpuCrash();
	RhiGpuCrash(const RhiGpuCrash&) = delete;
	RhiGpuCrash& operator=(const RhiGpuCrash&) = delete;

private:
	GpuCrashSession* m_session = nullptr;
};

#endif
