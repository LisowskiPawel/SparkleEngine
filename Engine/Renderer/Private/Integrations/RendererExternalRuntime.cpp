#include "PCH.h"
#include "Integrations/RendererExternalRuntime.h"

#include "Streamline/StreamlineRuntimeSupport.h"
#include "Core/Public/Diagnostics/Error.h"

RendererExternalRuntime::RendererExternalRuntime(RendererGraphicsLaunch graphicsLaunch)
{
	m_deviceLaunch.BackendApi = graphicsLaunch.BackendApi;
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #if SPARKLE_WITH_EXTERNAL_CAPTURE
	if (graphicsLaunch.GpuCrash.Mode != RhiGpuCrashMode::Off && graphicsLaunch.CaptureProvider != ExternalCaptureProvider::None)
	{
		throw Diagnostics::Error("Aftermath and active frame capture require separate launches.");
	}
  #endif
	if (graphicsLaunch.GpuCrash.Mode != RhiGpuCrashMode::Off)
	{
		m_gpuCrash = std::make_unique<RhiGpuCrash>(graphicsLaunch.BackendApi, graphicsLaunch.GpuCrash);
	}
#endif
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	if (graphicsLaunch.CaptureProvider != ExternalCaptureProvider::None)
	{
		m_externalCapture = std::make_unique<RhiExternalCapture>(graphicsLaunch.BackendApi, graphicsLaunch.CaptureProvider);
		m_deviceLaunch.ExternalCapture = m_externalCapture.get();
	}
#endif
	m_deviceLaunch.InterposerHooks = InitializeSharedStreamlineRuntime(m_deviceLaunch.BackendApi);
}

RendererExternalRuntime::~RendererExternalRuntime() noexcept
{
	m_owner.AssertAccess();
	ShutdownSharedStreamlineRuntime();
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	m_gpuCrash.reset();
#endif
}

const RhiDeviceLaunch& RendererExternalRuntime::GetDeviceLaunch() const noexcept
{
	m_owner.AssertAccess();
	return m_deviceLaunch;
}

void RendererExternalRuntime::BeginSimulationFrame(std::uint64_t frameId) noexcept
{
	m_owner.AssertAccess();
	SetSharedStreamlineFrameMarker(ERhiFrameLatencyMarker::SimulationStart, frameId);
}

void RendererExternalRuntime::EndSimulationFrame(std::uint64_t frameId) noexcept
{
	m_owner.AssertAccess();
	SetSharedStreamlineFrameMarker(ERhiFrameLatencyMarker::SimulationEnd, frameId);
}
