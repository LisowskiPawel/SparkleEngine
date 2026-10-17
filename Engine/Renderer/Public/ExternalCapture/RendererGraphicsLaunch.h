#pragma once

#include "RHI/Public/Core/RhiBackendApi.h"
#include "RHI/Public/Diagnostics/RhiExternalCapture.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "RHI/Public/Diagnostics/RhiGpuCrash.h"
#endif

struct RendererGraphicsLaunch final
{
	ERhiBackendApi BackendApi = ERhiBackendApi::Unknown;
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	RhiGpuCrashLaunch GpuCrash;
#endif
#if SPARKLE_TARGET_EDITOR && !SPARKLE_BUILD_SHIPPING
	ExternalCaptureProvider CaptureProvider = ExternalCaptureProvider::None;
#endif
};
