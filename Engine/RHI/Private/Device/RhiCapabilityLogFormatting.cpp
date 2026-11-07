#include "PCH.h"

#include "Device/RhiCapabilityLogFormatting.h"

#include <format>

std::string FormatBackendVersionInfo(const RhiBackendVersionInfo& version)
{
	if (!version.IsKnown())
	{
		return "backendVersion(unknown)";
	}

	return std::format("backendVersion(kind={} value={}.{}.{} packed={:#x})", RhiBackendVersionSemanticToString(version.Semantic), version.Major, version.Minor, version.Patch, version.PackedValue);
}

std::string FormatBackendDiagnosticsSupport(const RhiBackendDiagnosticsSupport& diagnostics)
{
	return std::format(
	    "diagnostics(validation={}, debugLayer={}, objectNames={}, gpuEvents={}, timestampQueries={}, debugMessages={}, "
	    "liveObjectReports={}, crashDiagnostics={})",
	    diagnostics.bValidationEnabled,
	    diagnostics.bSupportsDebugLayer,
	    diagnostics.bSupportsObjectNames,
	    diagnostics.bSupportsGpuEvents,
	    diagnostics.bSupportsTimestampQueries,
	    diagnostics.bSupportsDebugMessages,
	    diagnostics.bSupportsLiveObjectReports,
	    diagnostics.bSupportsCrashDiagnostics);
}

std::string FormatBackendMemorySupport(const RhiBackendMemorySupport& memory)
{
	return std::format("memorySupport(diagnostics={}, budgetQueries={}, residencyPressure={})", memory.bSupportsMemoryDiagnostics, memory.bSupportsBudgetQueries, memory.bSupportsResidencyPressure);
}

std::string FormatExternalFeatureInteropCapabilities(const RhiExternalFeatureInteropCapabilities& capabilities)
{
	return std::format(
	    "externalFeatureInterop(bridge={}, adapter='{}', vendorId={:#06x}, deviceId={:#06x}, driver='{}', nativeDevice={}, "
	    "nativeQueue={}, nativeCommandList={}, nativeResources={}, explicitStates={}, providerEval={}, runtimeChecks={})",
	    RhiExternalFeatureBridgeKindToString(capabilities.BridgeKind),
	    capabilities.Adapter.Name,
	    capabilities.Adapter.VendorId,
	    capabilities.Adapter.DeviceId,
	    capabilities.Adapter.DriverDescription,
	    capabilities.bExposesNativeDevice,
	    capabilities.bExposesNativeGraphicsQueue,
	    capabilities.bExposesNativeGraphicsCommandList,
	    capabilities.bExposesNativeResources,
	    capabilities.bSupportsExplicitResourceStates,
	    capabilities.bSupportsExternalProviderEvaluation,
	    capabilities.bSupportsRuntimeProviderChecks);
}
