#include "PCH.h"

#include "Diagnostics/RhiDiagnosticsComposition.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "Diagnostics/GpuCrash/GpuCrashSession.h"
#endif

#include <cassert>
#include <utility>

class RhiDiagnosticsComposition final : public RenderDiagnostics
{
public:
	RhiDiagnosticsComposition(
	    std::unique_ptr<RenderObjectDiagnostics> objectDiagnostics,
	    std::unique_ptr<RenderTimingDiagnostics> timingDiagnostics,
	    std::unique_ptr<RenderMessageDiagnostics> messageDiagnostics,
	    std::unique_ptr<RenderFailureDiagnostics> failureDiagnostics,
	    std::unique_ptr<RenderMemoryDiagnostics> memoryDiagnostics,
	    bool bSupportsGpuEvents) noexcept :
	    m_objectDiagnostics(std::move(objectDiagnostics)),
	    m_timingDiagnostics(std::move(timingDiagnostics)),
	    m_messageDiagnostics(std::move(messageDiagnostics)),
	    m_failureDiagnostics(std::move(failureDiagnostics)),
	    m_memoryDiagnostics(std::move(memoryDiagnostics)),
	    m_bSupportsGpuEvents(bSupportsGpuEvents)
	{
		assert(m_objectDiagnostics != nullptr);
	}

	RhiDiagnosticsCapabilities GetCapabilities() const noexcept override
	{
		bool bRequiresScopes = false;
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
		requiresScopes = GpuCrashSession::Current() != nullptr;
#endif
		return RhiDiagnosticsCapabilities{
		    .bSupportsObjectNames = m_objectDiagnostics->SupportsObjectNames(),
		    .bSupportsGpuEvents = m_bSupportsGpuEvents || bRequiresScopes,
		    .bRequiresBaselineScopes = bRequiresScopes,
		    .bSupportsTimestampQueries = GetTimingDiagnostics() != nullptr,
		    .bSupportsDebugMessages = GetMessageDiagnostics() != nullptr,
		    .bSupportsLiveObjectReports = m_failureDiagnostics != nullptr && m_failureDiagnostics->SupportsLiveObjectReports(),
		    .bSupportsCrashDiagnostics = m_failureDiagnostics != nullptr && m_failureDiagnostics->SupportsCrashDiagnostics(),
		    .bSupportsMemoryDiagnostics = m_memoryDiagnostics != nullptr,
		    .bSupportsMemoryBudgetQueries = m_memoryDiagnostics != nullptr && m_memoryDiagnostics->SupportsBudgetQueries()};
	}

	RenderObjectDiagnostics& GetObjectDiagnostics() noexcept override { return *m_objectDiagnostics; }

	const RenderObjectDiagnostics& GetObjectDiagnostics() const noexcept override { return *m_objectDiagnostics; }

	RenderTimingDiagnostics* GetTimingDiagnostics() noexcept override
	{
		return m_timingDiagnostics != nullptr && m_timingDiagnostics->SupportsTimestampQueries() ? m_timingDiagnostics.get() : nullptr;
	}

	const RenderTimingDiagnostics* GetTimingDiagnostics() const noexcept override
	{
		return m_timingDiagnostics != nullptr && m_timingDiagnostics->SupportsTimestampQueries() ? m_timingDiagnostics.get() : nullptr;
	}

	RenderMessageDiagnostics* GetMessageDiagnostics() noexcept override
	{
		return m_messageDiagnostics != nullptr && m_messageDiagnostics->SupportsDebugMessages() ? m_messageDiagnostics.get() : nullptr;
	}

	const RenderMessageDiagnostics* GetMessageDiagnostics() const noexcept override
	{
		return m_messageDiagnostics != nullptr && m_messageDiagnostics->SupportsDebugMessages() ? m_messageDiagnostics.get() : nullptr;
	}

	RenderFailureDiagnostics* GetFailureDiagnostics() noexcept override { return HasFailureDiagnostics() ? m_failureDiagnostics.get() : nullptr; }

	const RenderFailureDiagnostics* GetFailureDiagnostics() const noexcept override { return HasFailureDiagnostics() ? m_failureDiagnostics.get() : nullptr; }

	RenderMemoryDiagnostics* GetMemoryDiagnostics() noexcept override { return m_memoryDiagnostics.get(); }

	const RenderMemoryDiagnostics* GetMemoryDiagnostics() const noexcept override { return m_memoryDiagnostics.get(); }

private:
	bool HasFailureDiagnostics() const noexcept { return m_failureDiagnostics != nullptr && (m_failureDiagnostics->SupportsLiveObjectReports() || m_failureDiagnostics->SupportsCrashDiagnostics()); }

	std::unique_ptr<RenderObjectDiagnostics> m_objectDiagnostics;
	std::unique_ptr<RenderTimingDiagnostics> m_timingDiagnostics;
	std::unique_ptr<RenderMessageDiagnostics> m_messageDiagnostics;
	std::unique_ptr<RenderFailureDiagnostics> m_failureDiagnostics;
	std::unique_ptr<RenderMemoryDiagnostics> m_memoryDiagnostics;
	bool m_bSupportsGpuEvents = false;
};

std::unique_ptr<RenderDiagnostics> CreateRhiDiagnosticsComposition(
    std::unique_ptr<RenderObjectDiagnostics> objectDiagnostics,
    std::unique_ptr<RenderTimingDiagnostics> timingDiagnostics,
    std::unique_ptr<RenderMessageDiagnostics> messageDiagnostics,
    std::unique_ptr<RenderFailureDiagnostics> failureDiagnostics,
    std::unique_ptr<RenderMemoryDiagnostics> memoryDiagnostics,
    bool bSupportsGpuEvents)
{
	return std::make_unique<RhiDiagnosticsComposition>(
	    std::move(objectDiagnostics),
	    std::move(timingDiagnostics),
	    std::move(messageDiagnostics),
	    std::move(failureDiagnostics),
	    std::move(memoryDiagnostics),
	    bSupportsGpuEvents);
}
