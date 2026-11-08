#include "PCH.h"
#include "Startup/ApplicationGpuCrashStartup.h"
#include "GameApplication.h"
#include "Core/Public/Process/ProcessReadiness.h"
#include "Renderer/Public/ExternalCapture/RendererGraphicsLaunch.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "Concurrency/ApplicationTaskRuntime.h"
  #include "Diagnostics/GpuCrash/GpuCrashIncidentSink.h"
  #include "Diagnostics/GpuCrash/GpuCrashInputs.h"
#endif
#include "Core/Public/Process/CommandLineUtils.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Identifiers/Uuid.h"
#include "Core/Public/Strings/StringUtils.h"

#include <map>
#include <string_view>

GpuCrashStartup ResolveGpuCrashStartup()
{
	const auto arguments = CommandLine::ReadProcessArguments();
	std::map<std::string, std::string> options;
	for (std::size_t index = 1; index < arguments.size(); ++index)
	{
		const std::string& token = arguments[index];
		const auto separator = token.find('=');
		const auto name = token.substr(0, separator);
		if (name != "--gpu-crash-debugging" && name != "--shader-export" && name != "--gpu-crash-session")
		{
			continue;
		}
		std::string value;
		if (separator != std::string::npos)
		{
			value = token.substr(separator + 1);
		}
		else if (++index < arguments.size())
		{
			value = arguments[index];
		}
		if (value.empty() || value.starts_with("--") || !options.emplace(name, std::move(value)).second)
		{
			throw Diagnostics::Error("GPU crash startup options require one nonempty value each; duplicates are rejected.");
		}
	}
	const auto mode = options.find("--gpu-crash-debugging");
	if (mode == options.end() || mode->second == "off")
	{
		if (options.contains("--shader-export") || options.contains("--gpu-crash-session"))
		{
			throw Diagnostics::Error("Shader export and session options require Aftermath mode.");
		}
		return {};
	}
	if (mode->second != "aftermath" || !options.contains("--shader-export"))
	{
		throw Diagnostics::Error("Use --gpu-crash-debugging off|aftermath; Aftermath requires the exact --shader-export manifest.");
	}
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	GpuCrashStartup startup;
	startup.ShaderExport = Strings::ToWide(std::string_view{options.at("--shader-export")});
	startup.SessionId = options.contains("--gpu-crash-session") ? options.at("--gpu-crash-session") : Identifiers::CreateUuidV4String();
	const bool bInvalidCrashStartupShaderExport = !startup.ShaderExport.is_absolute() || startup.ShaderExport.filename() != "shader-export.json";
	const bool bInvalidGpuCrashStartup = bInvalidCrashStartupShaderExport || (!Identifiers::IsUuidV4String(startup.SessionId));

	if (bInvalidGpuCrashStartup)
	{
		throw Diagnostics::Error("Aftermath requires an absolute shader-export.json path and a valid fresh session UUID.");
	}
	return startup;
#else
	throw Diagnostics::Error("GPU crash debugging requires the explicit diagnostic product variant.");
#endif
}

void GameApplication::PrepareGpuCrashDiagnostics(RendererGraphicsLaunch& graphics)
{
	[[maybe_unused]] const GpuCrashStartup startup = ResolveGpuCrashStartup();
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	if (!startup.ShaderExport.empty())
	{
		graphics.GpuCrash.LibraryPath = ResolveAftermathLibraryPath();
		m_gpuCrashSink = std::make_unique<GpuCrashIncidentSink>(*m_taskRuntime, startup, graphics.BackendApi);
		graphics.GpuCrash.Mode = RhiGpuCrashMode::Aftermath;
		graphics.GpuCrash.Sink = m_gpuCrashSink.get();
	}
#else
	(void) graphics;
#endif
}

void GameApplication::AbortGpuCrashDiagnostics() noexcept
{
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	if (m_gpuCrashSink)
	{
		m_gpuCrashSink->AbortPreparation();
		m_gpuCrashSink.reset();
	}
#endif
}

void GameApplication::SignalLevelReadiness(std::string_view level)
{
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	if (m_gpuCrashSink)
	{
		m_pendingReadiness = level;
		return;
	}
#endif
	Process::SignalParentReadiness(level);
}

#if SPARKLE_WITH_GPU_CRASH_DEBUGGING

void GameApplication::ReportReadiness()
{
	if (m_pendingReadiness.empty())
	{
		return;
	}
	if (m_gpuCrashSink && !m_gpuCrashSink->IsReadyPublished())
	{
		return;
	}
	Process::SignalParentReadiness(m_pendingReadiness);
	m_pendingReadiness.clear();
}
#endif
