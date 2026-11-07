#pragma once

#include <cstdint>
#include <string>

namespace SparkleLauncher
{
	enum class HostGraphicsVendor : std::uint8_t
	{
		Unknown,
		Nvidia,
		Amd,
		Intel,
		Other
	};

	struct HostGraphicsCapabilities
	{
		HostGraphicsVendor PrimaryVendor = HostGraphicsVendor::Unknown;
		bool bHasNvidiaAdapter = false;
		bool bHasAmdAdapter = false;
		bool bHasIntelAdapter = false;
		bool bAdapterDetected = false;
		std::string Summary;
	};

	const HostGraphicsCapabilities& GetHostGraphicsCapabilities();
}
