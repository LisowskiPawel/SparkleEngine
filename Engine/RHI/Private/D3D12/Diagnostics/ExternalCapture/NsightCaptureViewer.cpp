#include "PCH.h"
#include "NsightCaptureViewer.h"

#include <Windows.h>
#include <ShlObj.h>
#include <ShlDisp.h>
#include <wrl/client.h>

#if SPARKLE_RHI_WITH_NSIGHT_CAPTURE
bool OpenNsightCaptureInDesktopShell(const wchar_t* path)
{
	// Spawn through Explorer, outside the target's inherited capture hooks.
	// Creating the viewer directly would inject Nsight into its own D3D9 UI.
	const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(initialized) && initialized != RPC_E_CHANGED_MODE)
	{
		return false;
	}

	bool bOpened = false;
	{
		using Microsoft::WRL::ComPtr;
		ComPtr<IShellWindows> windows;
		ComPtr<IDispatch> desktop;
		ComPtr<IServiceProvider> services;
		ComPtr<IShellBrowser> browser;
		ComPtr<IShellView> view;
		ComPtr<IDispatch> background;
		ComPtr<IShellFolderViewDual> folder;
		ComPtr<IDispatch> application;
		ComPtr<IShellDispatch2> shell;
		VARIANT empty{};
		long window = 0;
		const HRESULT windowsStatus = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&windows));
		const bool bDesktopFound = SUCCEEDED(windowsStatus) && windows->FindWindowSW(&empty, &empty, SWC_DESKTOP, &window, SWFO_NEEDDISPATCH, &desktop) == S_OK;
		const bool bServicesAvailable = bDesktopFound && SUCCEEDED(desktop.As(&services));
		const bool bBrowserAvailable = bServicesAvailable && SUCCEEDED(services->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser)));
		const bool bViewAvailable = bBrowserAvailable && SUCCEEDED(browser->QueryActiveShellView(&view));
		const bool bBackgroundAvailable = bViewAvailable && SUCCEEDED(view->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&background)));
		const bool bFolderAvailable = bBackgroundAvailable && SUCCEEDED(background.As(&folder));
		const bool bApplicationAvailable = bFolderAvailable && SUCCEEDED(folder->get_Application(&application));
		const bool bShellAvailable = bApplicationAvailable && SUCCEEDED(application.As(&shell));

		if (bShellAvailable)
		{
			if (BSTR filename = SysAllocString(path))
			{
				bOpened = SUCCEEDED(shell->ShellExecute(filename, empty, empty, empty, empty));
				SysFreeString(filename);
			}
		}
	}
	if (SUCCEEDED(initialized))
	{
		CoUninitialize();
	}

	return bOpened;
}

#endif
