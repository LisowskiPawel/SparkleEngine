#pragma once

#include "RHI/Public/Diagnostics/ExternalCaptureProvider.h"

namespace ExternalToolActivity
{
	bool HasGraphicsCapture() noexcept;
	bool HasTrace() noexcept;
	bool IsCaptureProviderLoaded(ExternalCaptureProvider provider) noexcept;
}
