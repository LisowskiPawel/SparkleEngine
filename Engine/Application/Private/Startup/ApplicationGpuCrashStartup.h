#pragma once

#include <filesystem>
#include <string>

struct GpuCrashStartup final
{
	std::filesystem::path ShaderExport;
	std::string SessionId;
};

GpuCrashStartup ResolveGpuCrashStartup();
