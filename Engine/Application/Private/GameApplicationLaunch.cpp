#include "PCH.h"

#include "GameApplicationLaunch.h"

#include "Application.h"
#include "GameApplication.h"
#include "Core/Public/Threading/ThreadOwnership.h"

int RunGameApplication()
{
	return RunGameApplication(GameApplicationOptions{});
}

int RunGameApplication(GameApplicationOptions options)
{
	Threading::SetCurrentThreadRole("Sparkle.GameThread");
	Application::ConfigureProcessFromCommandLine();

	GameApplication app(options);
	app.Run();
	return 0;
}
