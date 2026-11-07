#include "PCH.h"
#include "Device/RhiCapabilitySupport.h"

#include "Diagnostics/RhiDiagnostics.h"
#include "Memory/RhiMemoryDiagnostics.h"

RhiBackendDiagnosticsSupport BuildRhiBackendDiagnosticsSupport(bool bValidationEnabled, RenderDiagnostics& renderDiagnostics) noexcept
{
	const RhiDiagnosticsCapabilities diagnosticsCapabilities = renderDiagnostics.GetCapabilities();

	return RhiBackendDiagnosticsSupport{
	    .bValidationEnabled = bValidationEnabled,
	    .bSupportsDebugLayer = bValidationEnabled,
	    .bSupportsObjectNames = diagnosticsCapabilities.bSupportsObjectNames,
	    .bSupportsGpuEvents = diagnosticsCapabilities.bSupportsGpuEvents,
	    .bSupportsTimestampQueries = diagnosticsCapabilities.bSupportsTimestampQueries,
	    .bSupportsDebugMessages = diagnosticsCapabilities.bSupportsDebugMessages,
	    .bSupportsLiveObjectReports = diagnosticsCapabilities.bSupportsLiveObjectReports,
	    .bSupportsCrashDiagnostics = diagnosticsCapabilities.bSupportsCrashDiagnostics};
}

RhiBackendMemorySupport BuildRhiBackendMemorySupport(RenderDiagnostics& renderDiagnostics) noexcept
{
	const RenderMemoryDiagnostics* const memoryDiagnostics = renderDiagnostics.GetMemoryDiagnostics();

	return RhiBackendMemorySupport{
	    .bSupportsMemoryDiagnostics = memoryDiagnostics != nullptr,
	    .bSupportsBudgetQueries = memoryDiagnostics != nullptr && memoryDiagnostics->SupportsBudgetQueries(),
	    .bSupportsDelayedDestructionTracking = memoryDiagnostics != nullptr && memoryDiagnostics->SupportsDelayedDestructionTracking(),
	    .bSupportsResidencyPressure = memoryDiagnostics != nullptr && memoryDiagnostics->SupportsBudgetQueries()};
}
