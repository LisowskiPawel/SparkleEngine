#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace SparkleLauncher
{
	struct GpuCrashIncident final
	{
		std::filesystem::path ManifestPath;
		std::string SessionId;
		std::uint32_t ChildPid = 0;
	};
}
