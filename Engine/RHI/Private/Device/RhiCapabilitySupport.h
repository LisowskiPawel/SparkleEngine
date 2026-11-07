#pragma once

#include "Core/RhiCapabilities.h"

class RenderDiagnostics;

RhiBackendDiagnosticsSupport BuildRhiBackendDiagnosticsSupport(bool bValidationEnabled, RenderDiagnostics& renderDiagnostics) noexcept;
RhiBackendMemorySupport BuildRhiBackendMemorySupport(RenderDiagnostics& renderDiagnostics) noexcept;
