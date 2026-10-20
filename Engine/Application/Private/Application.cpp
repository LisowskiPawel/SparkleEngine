#include "PCH.h"

#include "Application.h"

#include "ApplicationCommandLineCVars.h"
#include "Concurrency/ConcurrencyLaunchCVars.h"
#include "Concurrency/TaskRuntimeCVars.h"
#include "Core/Public/Environment/EnvironmentVariables.h"
#include "RenderingSettings/EngineRenderingSettingsPersistence.h"

#include <cstdlib>

void Application::ConfigureProcessFromCommandLine()
{
	if (Environment::GetFlag("SPARKLE_SUPPRESS_CRASH_DIALOGS"))
	{
		SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	}

	ConcurrencyLaunchCVars::Register();
	TaskRuntimeCVars::Register();
	EngineRenderingSettingsPersistence::Apply();
#if !SPARKLE_BUILD_SHIPPING
	ApplicationCommandLineCVars::Apply();
#endif
}

void Application::Run()
{
	try
	{
		Initialize();
		for (;;)
		{
			if (!Tick())
			{
				break;
			}
		}
	}
	catch (...)
	{
		Shutdown();
		throw;
	}
	Shutdown();
}
