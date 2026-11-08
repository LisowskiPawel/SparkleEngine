#include "PCH.h"

#include "Application.h"

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
