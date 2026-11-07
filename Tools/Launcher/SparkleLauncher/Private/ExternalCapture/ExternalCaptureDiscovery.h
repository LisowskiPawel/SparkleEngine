#pragma once

#include "RHI/Public/Diagnostics/ExternalCaptureProvider.h"
#include <string>
#include <string_view>

namespace SparkleLauncher
{
	struct ExternalCaptureAvailability final
	{
		bool bInstalled = false;
		bool bSupported = false;
		std::string Detail;

		bool Available() const noexcept { return bInstalled && bSupported; }
	};

	ExternalCaptureAvailability InspectExternalCaptureProvider(ExternalCaptureProvider provider, std::string_view graphicsApi, std::string_view productProfile);
}
