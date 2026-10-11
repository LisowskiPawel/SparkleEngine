#include "PCH.h"

#include "GameApplicationLaunch.h"

#include "Application.h"
#include "GameApplication.h"
#include "Core/Public/Threading/ThreadOwnership.h"
#include <exception>

int RunGameApplication()
{
	return RunGameApplication(GameApplicationOptions{});
}

int RunGameApplication(GameApplicationOptions options)
{
	Threading::SetCurrentThreadRole("Sparkle.GameThread");
	try
	{
		Application::ConfigureProcessFromCommandLine();
		GameApplication app(options);
		app.Run();
		return 0;
	}
	catch (const std::exception& error)
	{
		MessageBoxA(nullptr, error.what(), "Sparkle Game could not complete the operation", MB_OK | MB_ICONERROR);
		return 1;
	}
}
