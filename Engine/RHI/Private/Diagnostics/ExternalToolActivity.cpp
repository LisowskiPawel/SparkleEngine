#include "PCH.h"
#include "Diagnostics/ExternalToolActivity.h"

#include <Windows.h>

bool ExternalToolActivity::IsCaptureProviderLoaded(ExternalCaptureProvider provider) noexcept
{
	switch (provider)
	{
		case ExternalCaptureProvider::Pix:
			return GetModuleHandleW(L"WinPixGpuCapturer.dll") != nullptr;
		case ExternalCaptureProvider::RenderDoc:
			return GetModuleHandleW(L"renderdoc.dll") != nullptr;
		case ExternalCaptureProvider::NsightGraphics:
			return GetModuleHandleW(L"ngfx-capture-injection.dll") != nullptr || GetModuleHandleW(L"ngfx-capture-interception.dll") != nullptr;
		default:
			return false;
	}
}

bool ExternalToolActivity::HasGraphicsCapture() noexcept
{
	return IsCaptureProviderLoaded(ExternalCaptureProvider::Pix) || IsCaptureProviderLoaded(ExternalCaptureProvider::RenderDoc) || IsCaptureProviderLoaded(ExternalCaptureProvider::NsightGraphics);
}

bool ExternalToolActivity::HasTrace() noexcept
{
	return GetModuleHandleW(L"WarpVizTarget.dll") != nullptr || GetModuleHandleW(L"ToolsInjection64.dll") != nullptr;
}
