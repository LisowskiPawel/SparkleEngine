#include "PCH.h"

#include "GameApplicationLaunch.h"

#include "Host/Application.h"
#include "Game/GameApplication.h"
#include "Core/Public/Threading/ThreadOwnership.h"

#include <Windows.h>
#include <exception>

int RunGameApplication()
{
	Threading::SetCurrentThreadRole("Sparkle.GameThread");
	try
	{
		Application::ConfigureProcessFromCommandLine();
		GameApplication app;
		app.Run();
		return 0;
	}
	catch (const std::exception& error)
	{
		MessageBoxA(nullptr, error.what(), "Sparkle Game could not complete the operation", MB_OK | MB_ICONERROR);
		return 1;
	}
}
