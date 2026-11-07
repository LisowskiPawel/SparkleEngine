#include "Build/Toolchains/HostGraphicsCapabilities.h"

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

#include <cwctype>
#include <string_view>

namespace SparkleLauncher
{

#if defined(_WIN32)
	bool WideContainsInsensitive(std::wstring_view text, std::wstring_view needle)
	{
		if (needle.empty() || needle.size() > text.size())
		{
			return false;
		}

		for (std::size_t offset = 0; offset + needle.size() <= text.size(); ++offset)
		{
			bool bMatches = true;
			for (std::size_t index = 0; index < needle.size(); ++index)
			{
				if (std::towupper(text[offset + index]) != std::towupper(needle[index]))
				{
					bMatches = false;
					break;
				}
			}
			if (bMatches)
			{
				return true;
			}
		}

		return false;
	}

	HostGraphicsVendor DetectVendorFromDeviceId(std::wstring_view deviceId)
	{
		if (WideContainsInsensitive(deviceId, L"VEN_10DE"))
		{
			return HostGraphicsVendor::Nvidia;
		}
		if (WideContainsInsensitive(deviceId, L"VEN_1002") || WideContainsInsensitive(deviceId, L"VEN_1022"))
		{
			return HostGraphicsVendor::Amd;
		}
		if (WideContainsInsensitive(deviceId, L"VEN_8086"))
		{
			return HostGraphicsVendor::Intel;
		}
		return HostGraphicsVendor::Other;
	}

	void MarkVendor(HostGraphicsCapabilities& capabilities, HostGraphicsVendor vendor, bool bPrimaryAdapter)
	{
		switch (vendor)
		{
			case HostGraphicsVendor::Nvidia:
				capabilities.bHasNvidiaAdapter = true;

				if (bPrimaryAdapter)
				{
					capabilities.PrimaryVendor = HostGraphicsVendor::Nvidia;
				}
				return;

			case HostGraphicsVendor::Amd:
				capabilities.bHasAmdAdapter = true;

				if (bPrimaryAdapter)
				{
					capabilities.PrimaryVendor = HostGraphicsVendor::Amd;
				}
				return;

			case HostGraphicsVendor::Intel:
				capabilities.bHasIntelAdapter = true;

				if (bPrimaryAdapter)
				{
					capabilities.PrimaryVendor = HostGraphicsVendor::Intel;
				}
				return;
			case HostGraphicsVendor::Other:
				if (bPrimaryAdapter)
				{
					capabilities.PrimaryVendor = HostGraphicsVendor::Other;
				}
				return;
			case HostGraphicsVendor::Unknown:
				return;
		}
	}
#endif

	HostGraphicsCapabilities DetectHostGraphicsCapabilities()
	{
		HostGraphicsCapabilities capabilities;

#if defined(_WIN32)
		for (DWORD deviceIndex = 0;; ++deviceIndex)
		{
			DISPLAY_DEVICEW adapter{};
			adapter.cb = sizeof(adapter);
			if (!EnumDisplayDevicesW(nullptr, deviceIndex, &adapter, 0))
			{
				break;
			}

			if ((adapter.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0)
			{
				continue;
			}

			const HostGraphicsVendor vendor = DetectVendorFromDeviceId(adapter.DeviceID);
			const bool bPrimaryAdapter = (adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;
			capabilities.bAdapterDetected = true;
			MarkVendor(capabilities, vendor, bPrimaryAdapter);
		}

		if (capabilities.PrimaryVendor == HostGraphicsVendor::Unknown)
		{
			if (capabilities.bHasNvidiaAdapter)
			{
				capabilities.PrimaryVendor = HostGraphicsVendor::Nvidia;
			}
			else if (capabilities.bHasAmdAdapter)
			{
				capabilities.PrimaryVendor = HostGraphicsVendor::Amd;
			}
			else if (capabilities.bHasIntelAdapter)
			{
				capabilities.PrimaryVendor = HostGraphicsVendor::Intel;
			}
			else if (capabilities.bAdapterDetected)
			{
				capabilities.PrimaryVendor = HostGraphicsVendor::Other;
			}
		}
#endif

		if (capabilities.bHasNvidiaAdapter)
		{
			capabilities.Summary = capabilities.bHasAmdAdapter ? "Detected NVIDIA and AMD graphics adapters; NVIDIA SDK dependencies stay enabled."
			                                                   : "Detected NVIDIA graphics adapter; NVIDIA SDK dependencies stay enabled.";
		}
		else if (capabilities.bHasAmdAdapter)
		{
			capabilities.Summary = "Detected AMD graphics adapter; NVIDIA SDK dependencies stay disabled.";
		}
		else if (capabilities.bHasIntelAdapter)
		{
			capabilities.Summary = "Detected Intel graphics adapter; NVIDIA SDK dependencies stay disabled.";
		}
		else if (capabilities.bAdapterDetected)
		{
			capabilities.Summary = "Detected graphics adapter, but it does not identify as NVIDIA or AMD; NVIDIA SDK dependencies stay disabled.";
		}
		else
		{
			capabilities.Summary = "Graphics adapter vendor could not be detected; NVIDIA SDK dependencies stay disabled by default.";
		}

		return capabilities;
	}

	const HostGraphicsCapabilities& GetHostGraphicsCapabilities()
	{
		static const HostGraphicsCapabilities capabilities = DetectHostGraphicsCapabilities();
		return capabilities;
	}
}
