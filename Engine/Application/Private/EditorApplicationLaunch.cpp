#include "PCH.h"

#include "EditorApplicationLaunch.h"

#include "Application.h"
#include "EditorApplication.h"
#include "Core/Public/Threading/ThreadOwnership.h"
#include <exception>

int RunEditorApplication()
{
	Threading::SetCurrentThreadRole("Sparkle.EditorThread");
	try
	{
		Application::ConfigureProcessFromCommandLine();
		EditorApplication app;
		app.Run();
		return 0;
	}
	catch (const std::exception& error)
	{
		MessageBoxA(nullptr, error.what(), "Sparkle Editor could not complete the operation", MB_OK | MB_ICONERROR);
		return 1;
	}
}
