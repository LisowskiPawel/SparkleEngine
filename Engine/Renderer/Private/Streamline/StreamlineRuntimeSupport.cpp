#include "../PCH.h"
#include "Streamline/StreamlineRuntimeSupport.h"

#if SPARKLE_WITH_NVIDIA_STREAMLINE
  #include <sl_dlss.h>
  #include <sl_dlss_d.h>
  #include <sl_helpers.h>
  #include <sl_pcl.h>
  #include <sl_reflex.h>

  #include <condition_variable>
  #include <mutex>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_streamlineRuntimeLogger, "Renderer.Streamline.Runtime");

class StreamlineRuntime final
{
public:
	static bool Initialize(ERhiBackendApi backendApi)
	{
		if (backendApi != ERhiBackendApi::D3D12)
		{
			return false;
		}

		{
			std::lock_guard lock(s_mutex);
			if (s_bInitialized)
			{
				return true;
			}
			if (s_bInitializing || s_bShuttingDown)
			{
				return false;
			}
			s_bInitializing = true;
		}

		sl::Preferences preferences{};
		FillPreferences(preferences);
		const bool bInitialized = slInit(preferences, sl::kSDKVersion) == sl::Result::eOk;
		{
			std::lock_guard lock(s_mutex);
			s_bInitializing = false;
			s_bInitialized = bInitialized;
		}
		s_idle.notify_all();
		return bInitialized;
	}

	static RhiInterposerHooks GetInterposerHooks() noexcept
	{
		std::lock_guard lock(s_mutex);
		if (!s_bInitialized || s_bShuttingDown)
		{
			return {};
		}

		return RhiInterposerHooks{
		    .DeviceCreated = &SetNativeDevice,
		    .UpgradeInterface = &UpgradeInterface,
		    .ResolveNativeInterface = &ResolveNativeInterface,
		    .PresentationReady = &SetPresentationReady,
		    .FrameMarker = &SetFrameMarker,
		    .RuntimeShutdown = &ShutdownFromInterposer};
	}

	static void Shutdown() noexcept
	{
		bool bInitialized = false;
		{
			std::unique_lock lock(s_mutex);
			s_bShuttingDown = true;
			s_idle.wait(lock, [] { return s_activeCalls == 0 && !s_bInitializing; });
			bInitialized = s_bInitialized;
		}
		if (bInitialized)
		{
			(void) slShutdown();
		}
		{
			std::lock_guard lock(s_mutex);
			s_bInitialized = false;
			s_bDeviceBound = false;
			s_bPresentationReady = false;
			s_bPclSupported = false;
			s_bReflexSupported = false;
			s_bShuttingDown = false;
		}
	}

	static bool IsFeatureSupported(sl::Feature feature, const RhiCapabilities& capabilities, RhiNativeDeviceQueueInterop nativeInterop) noexcept
	{
		if (!ValidateBackend(capabilities, nativeInterop))
		{
			return false;
		}

		CallLease call(CallRequirement::RuntimeReady);
		if (!call)
		{
			return false;
		}

		const RhiAdapterIdentity& adapter = capabilities.ExternalFeatureInterop.Adapter;
		auto luid = adapter.NativeLuid;
		sl::AdapterInfo adapterInfo{};
		adapterInfo.deviceLUID = luid.data();
		adapterInfo.deviceLUIDSizeInBytes = adapter.NativeLuidSizeInBytes;
		return slIsFeatureSupported(feature, adapterInfo) == sl::Result::eOk;
	}

	static void SetApplicationFrameMarker(ERhiFrameLatencyMarker marker, std::uint64_t frameId) noexcept { SetFrameMarker(marker, frameId, nullptr); }

private:
	enum class CallRequirement : std::uint8_t
	{
		Initialized,
		DeviceBound,
		RuntimeReady,
	};

	class CallLease final
	{
	public:
		explicit CallLease(CallRequirement requirement) noexcept
		{
			std::lock_guard lock(s_mutex);
			if (!s_bShuttingDown && MeetsRequirement(requirement))
			{
				++s_activeCalls;
				m_bAcquired = true;
			}
		}

		~CallLease() noexcept
		{
			if (!m_bAcquired)
			{
				return;
			}
			{
				std::lock_guard lock(s_mutex);
				--s_activeCalls;
			}
			s_idle.notify_all();
		}

		explicit operator bool() const noexcept { return m_bAcquired; }

	private:
		bool m_bAcquired = false;
	};

	static bool MeetsRequirement(CallRequirement requirement) noexcept
	{
		if (!s_bInitialized)
		{
			return false;
		}
		if (requirement == CallRequirement::Initialized)
		{
			return true;
		}
		if (!s_bDeviceBound)
		{
			return false;
		}
		return requirement == CallRequirement::DeviceBound || s_bPresentationReady;
	}

	static bool SetNativeDevice(NativeGraphicsDeviceHandle nativeDevice, const RhiAdapterIdentity& adapter, void*) noexcept
	{
		CallLease call(CallRequirement::Initialized);
		if (!call || !nativeDevice)
		{
			return false;
		}

		const bool bDeviceBound = slSetD3DDevice(nativeDevice.Value) == sl::Result::eOk;
		bool bPclSupported = false;
		bool bReflexSupported = false;
		if (bDeviceBound && HasAdapterLuid(adapter))
		{
			auto adapterLuid = adapter.NativeLuid;
			sl::AdapterInfo adapterInfo{};
			adapterInfo.deviceLUID = adapterLuid.data();
			adapterInfo.deviceLUIDSizeInBytes = adapter.NativeLuidSizeInBytes;
			bPclSupported = slIsFeatureSupported(sl::kFeaturePCL, adapterInfo) == sl::Result::eOk;
			bReflexSupported = slIsFeatureSupported(sl::kFeatureReflex, adapterInfo) == sl::Result::eOk;
			if (bReflexSupported && slReflexSetOptions(sl::ReflexOptions{}) != sl::Result::eOk)
			{
				Diagnostics::Fatal(g_streamlineRuntimeLogger, __FILE__, __LINE__, "Streamline Reflex rejected its initial latency configuration.");
			}
		}
		{
			std::lock_guard lock(s_mutex);
			s_bDeviceBound = bDeviceBound;
			s_bPclSupported = bPclSupported;
			s_bReflexSupported = bReflexSupported;
		}
		return bDeviceBound;
	}

	static bool UpgradeInterface(ERhiInterposerInterfaceKind, void** nativeInterface, void*) noexcept
	{
		CallLease call(CallRequirement::DeviceBound);
		return call && nativeInterface != nullptr && slUpgradeInterface(nativeInterface) == sl::Result::eOk;
	}

	static bool ResolveNativeInterface(ERhiInterposerInterfaceKind, void* externalInterface, void** nativeInterface, void*) noexcept
	{
		CallLease call(CallRequirement::Initialized);
		return call && externalInterface != nullptr && nativeInterface != nullptr && slGetNativeInterface(externalInterface, nativeInterface) == sl::Result::eOk;
	}

	static void SetPresentationReady(bool bReady, void*) noexcept
	{
		std::lock_guard lock(s_mutex);
		const bool bRuntimeActive = !s_bShuttingDown && s_bInitialized;
		const bool bDeviceReady = bRuntimeActive && s_bDeviceBound;
		s_bPresentationReady = bDeviceReady && bReady;
	}

	static sl::PCLMarker ToPclMarker(ERhiFrameLatencyMarker marker) noexcept
	{
		switch (marker)
		{
			case ERhiFrameLatencyMarker::SimulationStart:
				return sl::PCLMarker::eSimulationStart;
			case ERhiFrameLatencyMarker::SimulationEnd:
				return sl::PCLMarker::eSimulationEnd;
			case ERhiFrameLatencyMarker::RenderSubmitStart:
				return sl::PCLMarker::eRenderSubmitStart;
			case ERhiFrameLatencyMarker::RenderSubmitEnd:
				return sl::PCLMarker::eRenderSubmitEnd;
			case ERhiFrameLatencyMarker::PresentStart:
				return sl::PCLMarker::ePresentStart;
			case ERhiFrameLatencyMarker::PresentEnd:
				return sl::PCLMarker::ePresentEnd;
		}
		Diagnostics::Fatal(g_streamlineRuntimeLogger, __FILE__, __LINE__, "Unknown external frame marker.");
	}

	static void SetFrameMarker(ERhiFrameLatencyMarker marker, std::uint64_t frameId, void*) noexcept
	{
		CallLease call(CallRequirement::RuntimeReady);
		if (!call || !s_bPclSupported)
		{
			return;
		}

		const std::uint32_t frameIndex = static_cast<std::uint32_t>(frameId);
		sl::FrameToken* frameToken = nullptr;
		if (slGetNewFrameToken(frameToken, &frameIndex) != sl::Result::eOk || frameToken == nullptr)
		{
			Diagnostics::Fatal(g_streamlineRuntimeLogger, __FILE__, __LINE__, "Streamline could not resolve the logical frame token.");
		}
		if (marker == ERhiFrameLatencyMarker::SimulationStart && s_bReflexSupported && slReflexSleep(*frameToken) != sl::Result::eOk)
		{
			Diagnostics::Fatal(g_streamlineRuntimeLogger, __FILE__, __LINE__, "Streamline Reflex sleep failed for the active frame.");
		}
		if (slPCLSetMarker(ToPclMarker(marker), *frameToken) != sl::Result::eOk)
		{
			Diagnostics::Fatal(g_streamlineRuntimeLogger, __FILE__, __LINE__, "Streamline PCL rejected a frame marker.");
		}
	}

	static void ShutdownFromInterposer(void*) noexcept { Shutdown(); }

	static void FillPreferences(sl::Preferences& preferences)
	{
		static constexpr sl::Feature features[] = {sl::kFeatureDLSS, sl::kFeatureDLSS_RR, sl::kFeaturePCL, sl::kFeatureReflex};
		preferences = {};
		preferences.featuresToLoad = features;
		preferences.numFeaturesToLoad = static_cast<std::uint32_t>(std::size(features));

		preferences.flags = sl::PreferenceFlags::eDisableCLStateTracking | sl::PreferenceFlags::eUseManualHooking | sl::PreferenceFlags::eUseFrameBasedResourceTagging;

		preferences.engine = sl::EngineType::eCustom;
		preferences.engineVersion = "SparkleEngine-Development";
		preferences.projectId = "535041524B4C45454E47494E45303031";
		preferences.renderAPI = sl::RenderAPI::eD3D12;
	}

	static bool HasAdapterLuid(const RhiAdapterIdentity& adapter) noexcept { return adapter.NativeLuidSizeInBytes > 0u && adapter.NativeLuidSizeInBytes <= adapter.NativeLuid.size(); }

	static bool ValidateBackend(const RhiCapabilities& capabilities, RhiNativeDeviceQueueInterop nativeInterop) noexcept
	{
		const RhiExternalFeatureInteropCapabilities& interop = capabilities.ExternalFeatureInterop;

		const bool bCommonInteropGraphicsQueue = interop.bExposesNativeDevice && interop.bExposesNativeGraphicsQueue;
		const bool bCommonInteropCommandList = bCommonInteropGraphicsQueue && (interop.bExposesNativeGraphicsCommandList);
		const bool bCommonInteropNativeResources = bCommonInteropCommandList && (interop.bExposesNativeResources);
		const bool bCommonInterop = bCommonInteropNativeResources && (interop.bSupportsExternalProviderEvaluation);

		return capabilities.BackendApi == ERhiBackendApi::D3D12 && bCommonInterop && nativeInterop && HasAdapterLuid(interop.Adapter);
	}

	static std::mutex s_mutex;
	static std::condition_variable s_idle;
	static bool s_bInitialized;
	static bool s_bDeviceBound;
	static bool s_bPresentationReady;
	static bool s_bPclSupported;
	static bool s_bReflexSupported;
	static bool s_bInitializing;
	static bool s_bShuttingDown;
	static std::uint32_t s_activeCalls;
};

std::mutex StreamlineRuntime::s_mutex;
std::condition_variable StreamlineRuntime::s_idle;
bool StreamlineRuntime::s_bInitialized = false;
bool StreamlineRuntime::s_bDeviceBound = false;
bool StreamlineRuntime::s_bPresentationReady = false;
bool StreamlineRuntime::s_bPclSupported = false;
bool StreamlineRuntime::s_bReflexSupported = false;
bool StreamlineRuntime::s_bInitializing = false;
bool StreamlineRuntime::s_bShuttingDown = false;
std::uint32_t StreamlineRuntime::s_activeCalls = 0;
#endif

RhiInterposerHooks InitializeSharedStreamlineRuntime(ERhiBackendApi backendApi)
{
#if SPARKLE_WITH_NVIDIA_STREAMLINE
	if (!StreamlineRuntime::Initialize(backendApi))
	{
		return {};
	}
	return StreamlineRuntime::GetInterposerHooks();
#else
	(void) backendApi;
	return {};
#endif
}

void ShutdownSharedStreamlineRuntime() noexcept
{
#if SPARKLE_WITH_NVIDIA_STREAMLINE
	StreamlineRuntime::Shutdown();
#endif
}

void SetSharedStreamlineFrameMarker(ERhiFrameLatencyMarker marker, std::uint64_t frameId) noexcept
{
#if SPARKLE_WITH_NVIDIA_STREAMLINE
	StreamlineRuntime::SetApplicationFrameMarker(marker, frameId);
#else
	(void) marker;
	(void) frameId;
#endif
}

#if SPARKLE_WITH_NVIDIA_STREAMLINE
bool IsStreamlineFeatureSupported(sl::Feature feature, const RhiCapabilities& capabilities, RhiNativeDeviceQueueInterop nativeInterop) noexcept
{
	return StreamlineRuntime::IsFeatureSupported(feature, capabilities, nativeInterop);
}
#endif
