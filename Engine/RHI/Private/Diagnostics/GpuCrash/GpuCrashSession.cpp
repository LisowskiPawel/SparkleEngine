#include "PCH.h"
#include "Diagnostics/GpuCrash/GpuCrashSession.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonWriter.h"
#include "Diagnostics/ExternalToolActivity.h"

#include <algorithm>
#include <cstring>
#include <format>
#include <thread>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogGpuCrash, "RHI.GpuCrash");

static std::atomic<GpuCrashSession*> g_gpuCrashSession{nullptr};
static std::atomic_flag g_gpuCrashSessionClaimed;

RhiGpuCrash::RhiGpuCrash(ERhiBackendApi api, const RhiGpuCrashLaunch& launch)
{
	if (launch.Mode == RhiGpuCrashMode::Off)
	{
		return;
	}
	if (g_gpuCrashSessionClaimed.test_and_set())
	{
		throw Diagnostics::Error("GPU crash collection permits one session per process.");
	}

	// Callbacks and their DLL have no documented join barrier. Retain the anchor.
	m_session = new GpuCrashSession(api, launch);
	g_gpuCrashSession.store(m_session, std::memory_order_release);
}

RhiGpuCrash::~RhiGpuCrash()
{
	if (m_session != nullptr)
	{
		m_session->Shutdown();
	}
}

GpuCrashSession::GpuCrashSession(ERhiBackendApi api, const RhiGpuCrashLaunch& launch) :
    m_sink(launch.Sink),
    m_markers(std::make_unique<MarkerEntry[]>(MaximumMarkers))
{
	const bool bInvalidCrashSessionSink = launch.Mode != RhiGpuCrashMode::Aftermath || m_sink == nullptr;
	const bool bInvalidCrashSessionLibraryPath = bInvalidCrashSessionSink || (!launch.LibraryPath.is_absolute());
	const bool bInvalidCrashSessionApi = api != ERhiBackendApi::D3D12 && api != ERhiBackendApi::Vulkan;
	const bool bInvalidCrashSession = bInvalidCrashSessionLibraryPath || bInvalidCrashSessionApi;

	if (bInvalidCrashSession)
	{
		throw Diagnostics::Error("Aftermath requires an admitted API, absolute DLL path and prepared artifact sink.");
	}
	if (ExternalToolActivity::HasGraphicsCapture() || ExternalToolActivity::HasTrace())
	{
		throw Diagnostics::Error("Aftermath and injected capture/trace tools require separate launches.");
	}

	const auto incidentId = m_sink->GetIncidentId();
	if (incidentId.size() != m_incidentId.size() - 1)
	{
		throw Diagnostics::Error("GPU crash sink requires a fresh incident UUID.");
	}

	std::copy(incidentId.begin(), incidentId.end(), m_incidentId.begin());
	InitializeLibrary(api, launch.LibraryPath);

	constexpr std::string_view unknownMarker = "unknown: marker capacity, nesting or label limit; frame/recording unavailable";
	std::copy(unknownMarker.begin(), unknownMarker.end(), m_unknown.Text.begin());
#if SPARKLE_RHI_WITH_D3D12
	if (api == ERhiBackendApi::D3D12)
	{
		PrepareD3D12GpuCrash();
	}
#endif
	const auto watched = api == ERhiBackendApi::D3D12 ? GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_DX : GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_Vulkan;
	HMODULE callbackModule = nullptr;

	const bool bCallbackPinFailed = !GetModuleHandleExW(
	    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
	    reinterpret_cast<const wchar_t*>(&GpuCrashSession::DumpCallback),
	    &callbackModule);

	if (bCallbackPinFailed)
	{
		throw Diagnostics::Error("Cannot retain GPU crash callback code for the process lifetime.");
	}

	const bool bAftermathEnableFailed = m_enable(
	                                        GFSDK_Aftermath_Version_API,
	                                        watched,
	                                        GFSDK_Aftermath_GpuCrashDumpFeatureFlags_Default,
	                                        DumpCallback,
	                                        DebugCallback,
	                                        DescriptionCallback,
	                                        MarkerCallback,
	                                        this)
	    != GFSDK_Aftermath_Result_Success;

	if (bAftermathEnableFailed)
	{
		// Even a failed enable has no callback-quiescence guarantee. Do not unwind the anchor.
		Diagnostics::Fatal(LogGpuCrash, __FILE__, __LINE__, "Aftermath crash collection could not be enabled; requested launch rejected.");
	}
}

void GpuCrashSession::InitializeLibrary(ERhiBackendApi api, const std::filesystem::path& libraryPath)
{
	Hash::Sha256Digest hash;
	std::string error;
	if (!Hash::TrySha256File(libraryPath, hash, error) || Hash::Sha256ToHex(hash) != SPARKLE_AFTERMATH_RUNTIME_SHA256)
	{
		throw Diagnostics::Error("Aftermath DLL does not match the pinned input receipt.");
	}

	m_library = LoadLibraryExW(libraryPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (m_library == nullptr)
	{
		throw Diagnostics::Error("Cannot load the admitted Aftermath DLL and its dependencies.");
	}

	std::array<wchar_t, 32'768> loadedPath{};
	const DWORD count = GetModuleFileNameW(m_library, loadedPath.data(), static_cast<DWORD>(loadedPath.size()));
	const bool bLoadedPathValid = count != 0 && count < loadedPath.size();
	const bool bLibraryPathMatches = bLoadedPathValid && std::filesystem::equivalent(libraryPath, loadedPath.data());
	const bool bRuntimeHashed = bLibraryPathMatches && Hash::TrySha256File(loadedPath.data(), hash, error);
	const bool bRuntimeHashMatches = bRuntimeHashed && Hash::Sha256ToHex(hash) == SPARKLE_AFTERMATH_RUNTIME_SHA256;

	if (!bRuntimeHashMatches)
	{
		throw Diagnostics::Error("Loaded Aftermath DLL identity changed or resolved to another file.");
	}

	m_enable = reinterpret_cast<PFN_GFSDK_Aftermath_EnableGpuCrashDumps>(RequireExport("GFSDK_Aftermath_EnableGpuCrashDumps"));
	m_disable = reinterpret_cast<PFN_GFSDK_Aftermath_DisableGpuCrashDumps>(RequireExport("GFSDK_Aftermath_DisableGpuCrashDumps"));
	m_status = reinterpret_cast<PFN_GFSDK_Aftermath_GetCrashDumpStatus>(RequireExport("GFSDK_Aftermath_GetCrashDumpStatus"));
	m_debugIdentifier = reinterpret_cast<PFN_GFSDK_Aftermath_GetShaderDebugInfoIdentifier>(RequireExport("GFSDK_Aftermath_GetShaderDebugInfoIdentifier"));

	const std::array dxExports{
	    "GFSDK_Aftermath_DX12_Initialize",
	    "GFSDK_Aftermath_DX12_CreateContextHandle",
	    "GFSDK_Aftermath_DX12_UpdateResourceInfo",
	    "GFSDK_Aftermath_ReleaseContextHandle",
	    "GFSDK_Aftermath_SetEventMarker",
	    "GFSDK_Aftermath_GetShaderHash",
	    "GFSDK_Aftermath_GetShaderDebugName"};

	if (api == ERhiBackendApi::D3D12)
	{
		for (const char* name : dxExports)
		{
			RequireExport(name);
		}
	}
	else
	{
		RequireExport("GFSDK_Aftermath_GetShaderHashSpirv");
	}
}

GpuCrashSession* GpuCrashSession::Current() noexcept
{
	return g_gpuCrashSession.load(std::memory_order_acquire);
}

FARPROC GpuCrashSession::RequireExport(const char* name) const
{
	const auto function = GetProcAddress(m_library, name);
	if (function == nullptr)
	{
		throw Diagnostics::Error(std::format("Aftermath is missing required export {}.", name));
	}
	return function;
}

GpuCrashSession::SinkBorrow::SinkBorrow(GpuCrashSession& session) noexcept :
    m_session(session)
{
	std::lock_guard lock(session.m_sinkMutex);
	if (session.m_bAccepting && session.m_sink != nullptr)
	{
		m_sink = session.m_sink;
		++session.m_borrows;
	}
}

GpuCrashSession::SinkBorrow::~SinkBorrow()
{
	if (m_sink != nullptr)
	{
		std::lock_guard lock(m_session.m_sinkMutex);
		--m_session.m_borrows;
	}
}

RhiGpuCrashArtifactSink* GpuCrashSession::SinkBorrow::Get() const noexcept
{
	return m_sink;
}

void GpuCrashSession::Incomplete() noexcept
{
	m_bIncomplete.store(true, std::memory_order_relaxed);
	SinkBorrow borrow(*this);
	if (auto* sink = borrow.Get())
	{
		sink->MarkIncomplete();
	}
}

void GpuCrashSession::Ready() noexcept
{
	SinkBorrow borrow(*this);
	if (auto* sink = borrow.Get())
	{
		sink->MarkReady();
	}
}

void GpuCrashSession::Enqueue(RhiGpuCrashArtifactId id, std::span<const std::byte> bytes) noexcept
{
	SinkBorrow borrow(*this);
	if (auto* sink = borrow.Get())
	{
		if (!sink->Enqueue(id, bytes))
		{
			m_bIncomplete.store(true, std::memory_order_relaxed);
		}
	}
}

void GpuCrashSession::ShaderIdentity(std::span<const std::byte> code, std::uint64_t nativeHash, bool bHashAvailable, const GFSDK_Aftermath_ShaderDebugName* debugName) noexcept
{
	try
	{
		std::string sha256, error;
		if (!Hash::TrySha256Hex(code, sha256, error))
		{
			Incomplete();
			return;
		}
		Json::ObjectWriter json;
		json.WriteString("codeSha256", sha256);
		json.WriteRaw("nativeShaderHash", bHashAvailable ? Json::QuoteHexUInt64(nativeHash) : "null");
		if (debugName != nullptr)
		{
			json.WriteString("nativeDebugName", std::string_view(debugName->name, strnlen(debugName->name, sizeof(debugName->name))));
		}
		else
		{
			json.WriteRaw("nativeDebugName", "null");
		}
		json.WriteRaw("compilerLookupKey", "null");
		json.WriteRaw("cpuPdbIdentity", "null");
		const auto text = json.Finish();
		Enqueue({RhiGpuCrashArtifactKind::ShaderIdentity, m_shaderSequence.fetch_add(1), 0}, std::as_bytes(std::span(text)));
	}
	catch (...)
	{
		Incomplete();
	}
}

bool GpuCrashSession::BeginLoss(std::int64_t nativeFailure) noexcept
{
	if (m_bLossClaimed.exchange(true))
	{
		return false;
	}
	m_lossDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
	SinkBorrow borrow(*this);
	if (auto* sink = borrow.Get())
	{
		sink->BeginCollection(nativeFailure);
	}
	return true;
}

void GpuCrashSession::FinishLoss(std::int64_t nativeFailure) noexcept
{
	Settle(true, nativeFailure);
}

void GpuCrashSession::Shutdown() noexcept
{
	Settle(false, 0);
}

bool GpuCrashSession::SealSink(std::chrono::steady_clock::time_point deadline) noexcept
{
	for (;;)
	{
		{
			std::lock_guard lock(m_sinkMutex);
			m_bAccepting = false;
			if (m_borrows == 0)
			{
				return true;
			}
		}
		if (std::chrono::steady_clock::now() >= deadline)
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void GpuCrashSession::Settle(bool bLoss, std::int64_t nativeFailure) noexcept
{
	const auto deadline = bLoss ? m_lossDeadline : std::chrono::steady_clock::now() + std::chrono::seconds(10);
	bool bFinished = false;
	if (bLoss)
	{
		while (std::chrono::steady_clock::now() < deadline)
		{
			GFSDK_Aftermath_CrashDump_Status status;
			if (m_status(&status) != GFSDK_Aftermath_Result_Success || status == GFSDK_Aftermath_CrashDump_Status_CollectingDataFailed)
			{
				break;
			}
			if (status == GFSDK_Aftermath_CrashDump_Status_Finished)
			{
				bFinished = true;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	if (m_disable() != GFSDK_Aftermath_Result_Success)
	{
		Incomplete();
	}
	if (!SealSink(deadline))
	{
		if (bLoss)
		{
			// The host preserves its original terminal native error.
			return;
		}
		Diagnostics::Fatal(LogGpuCrash, __FILE__, __LINE__, "GPU crash callbacks did not settle before the deadline; retaining sink storage until process exit.");
	}

	// No callback can borrow Application storage past this point. The anchor remains.
	auto* sink = m_sink;
	if (m_bIncomplete.load(std::memory_order_relaxed))
	{
		sink->MarkIncomplete();
	}

	sink->Finalize(bFinished, std::chrono::steady_clock::now() >= deadline, nativeFailure);
	while (!sink->IsSettled() && std::chrono::steady_clock::now() < deadline)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (!sink->IsSettled())
	{
		if (bLoss)
		{
			// The host preserves its original terminal native error.
			return;
		}
		Diagnostics::Fatal(LogGpuCrash, __FILE__, __LINE__, "GPU crash writer did not settle before the deadline; retaining its storage until process exit.");
	}

	std::lock_guard lock(m_sinkMutex);
	m_sink = nullptr;
}
