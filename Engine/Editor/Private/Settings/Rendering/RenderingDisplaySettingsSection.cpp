#include "PCH.h"

#include "Settings/Rendering/RenderingDisplaySettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawDisplaySettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	static constexpr RenderingSettingsUi::ComboOption<PixelFormat> backBufferFormatOptions[] = {
	    {"R8G8B8A8 UNorm", PixelFormat::R8G8B8A8_UNorm},
	    {"R8G8B8A8 sRGB", PixelFormat::R8G8B8A8_UNorm_Srgb},
	    {"B8G8R8A8 UNorm", PixelFormat::B8G8R8A8_UNorm},
	    {"B8G8R8A8 sRGB", PixelFormat::B8G8R8A8_UNorm_Srgb},
	};

	static constexpr RenderingSettingsUi::ComboOption<EngineOutputColorEncoding> outputColorEncodingOptions[] = {
	    {"Automatic", EngineOutputColorEncoding::Automatic},
	    {"Linear", EngineOutputColorEncoding::Linear},
	    {"sRGB", EngineOutputColorEncoding::Srgb},
	};

	const bool displayMatches = RenderingSettingsUi::MatchesFilter(filterText, "Display", "display vsync high-performance adapter gpu back buffer format sdr srgb linear output encoding");
	const bool displayOpen = displayMatches && RenderingSettingsUi::BeginSettingsCategory("Display");

	if (!displayOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingDisplaySettings"))
	{
		bool vSync = settings.VSync;
		const bool vSyncEdited = RenderingSettingsUi::DrawBooleanRow("##VSync", "VSync", vSync);
		if (vSyncEdited)
		{
			settingsController.SetVSync(vSync);
		}

		RenderingSettingsUi::DrawComboOptionRow(
		    "##BackBufferFormat",
		    "Back buffer format",
		    settings.BackBufferFormat,
		    backBufferFormatOptions,
		    [&settingsController](PixelFormat value) { settingsController.SetBackBufferFormat(value); });

		bool preferHighPerformanceAdapter = settings.PreferHighPerformanceAdapter;
		const bool preferHighPerformanceAdapterEdited = RenderingSettingsUi::DrawBooleanRow("##PreferHighPerformanceAdapter", "Prefer high-performance adapter", preferHighPerformanceAdapter);
		if (preferHighPerformanceAdapterEdited)
		{
			settingsController.SetPreferHighPerformanceAdapter(preferHighPerformanceAdapter);
		}

		RenderingSettingsUi::DrawComboOptionRow(
		    "##OutputColorEncoding",
		    "Output encoding",
		    settings.OutputColorEncoding,
		    outputColorEncodingOptions,
		    [&settingsController](EngineOutputColorEncoding value) { settingsController.SetOutputColorEncoding(value); });

		ImGui::EndTable();
	}
	ImGui::Dummy(ImVec2(0.0f, 4.0f));
}
