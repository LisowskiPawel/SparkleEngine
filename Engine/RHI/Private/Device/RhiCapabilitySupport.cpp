#include "PCH.h"
#include "Device/RhiCapabilitySupport.h"

#include "Diagnostics/RhiDiagnostics.h"
#include "Memory/RhiMemoryDiagnostics.h"

RhiBackendDiagnosticsSupport BuildRhiBackendDiagnosticsSupport(bool validationEnabled, RenderDiagnostics& renderDiagnostics) noexcept
{
	const RhiDiagnosticsCapabilities diagnosticsCapabilities = renderDiagnostics.GetCapabilities();

	return RhiBackendDiagnosticsSupport{
	    .ValidationEnabled = validationEnabled,
	    .SupportsDebugLayer = validationEnabled,
	    .SupportsObjectNames = diagnosticsCapabilities.SupportsObjectNames,
	    .SupportsGpuEvents = diagnosticsCapabilities.SupportsGpuEvents,
	    .SupportsTimestampQueries = diagnosticsCapabilities.SupportsTimestampQueries,
	    .SupportsDebugMessages = diagnosticsCapabilities.SupportsDebugMessages,
	    .SupportsLiveObjectReports = diagnosticsCapabilities.SupportsLiveObjectReports,
	    .SupportsCrashDiagnostics = diagnosticsCapabilities.SupportsCrashDiagnostics};
}

RhiBackendMemorySupport BuildRhiBackendMemorySupport(RenderDiagnostics& renderDiagnostics) noexcept
{
	const RenderMemoryDiagnostics* const memoryDiagnostics = renderDiagnostics.GetMemoryDiagnostics();

	return RhiBackendMemorySupport{
	    .SupportsMemoryDiagnostics = memoryDiagnostics != nullptr,
	    .SupportsBudgetQueries = memoryDiagnostics != nullptr && memoryDiagnostics->SupportsBudgetQueries(),
	    .SupportsDelayedDestructionTracking = memoryDiagnostics != nullptr && memoryDiagnostics->SupportsDelayedDestructionTracking(),
	    .SupportsResidencyPressure = memoryDiagnostics != nullptr && memoryDiagnostics->SupportsBudgetQueries()};
}
