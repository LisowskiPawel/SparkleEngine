#pragma once

#include "Concurrency/Control/RenderControlCompletion.h"
#include "Diagnostics/ExternalCapture/ExternalCaptureControl.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

#include <cstdint>
#include <memory>
#include <variant>

struct RenderResizeCommand final
{
	RenderViewportExtent Extent;
	bool bMinimized = false;
};

struct RenderViewportCommand final
{
	ViewportRenderRequest Request;
};

struct RenderReloadShadersCommand final
{
	std::shared_ptr<RenderControlCompletion> Completion;
};

struct RenderMemoryDiagnosticsCommand final
{
	std::shared_ptr<RenderControlCompletion> Completion;
};

struct RenderCaptureCommand final
{
	ViewportCaptureId Id;
	ViewportCaptureRequest Request;
};

struct RenderSettingsChangedCommand final
{
	EngineRenderingSettingsState Settings;
};

struct RenderCVarCommand final
{
	CVarControlRequest Request;
	std::shared_ptr<RenderControlCompletion> Completion;
};

struct RenderSettingsCaptureCommand final
{
	std::shared_ptr<RenderControlCompletion> Completion;
};

using RendererExecutionControl = std::variant<
    RenderResizeCommand,
    RenderViewportCommand,
    RenderReloadShadersCommand,
    RenderMemoryDiagnosticsCommand,
    RenderCaptureCommand,
    RenderSettingsChangedCommand,
    RenderCVarCommand,
    RenderSettingsCaptureCommand
#if SPARKLE_WITH_EXTERNAL_CAPTURE
    ,
    RenderExternalCaptureCommand
#endif
    >;
