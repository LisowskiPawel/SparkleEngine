#include "PCH.h"

#include "Settings/Rendering/RenderingDisplaySettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawDisplaySettingsSection(EngineRenderingSettingsController& settingsController, const char* filterText)
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

	const bool bDisplayOpen = RenderingSettingsUi::BeginSettingsCategory(filterText, "Display", "display vsync high-performance adapter gpu back buffer format sdr srgb linear output encoding");

	if (!bDisplayOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingDisplaySettings"))
	{
		RenderingSettingsUi::DrawBooleanRow("##VSync", "VSync", settingsController, &EngineRenderingSettingsState::bVSync);

		RenderingSettingsUi::DrawComboOptionRow("##BackBufferFormat", "Back buffer format", settingsController, &EngineRenderingSettingsState::BackBufferFormat, backBufferFormatOptions);

		RenderingSettingsUi::DrawBooleanRow("##PreferHighPerformanceAdapter", "Prefer high-performance adapter", settingsController, &EngineRenderingSettingsState::bPreferHighPerformanceAdapter);

		RenderingSettingsUi::DrawComboOptionRow("##OutputColorEncoding", "Output encoding", settingsController, &EngineRenderingSettingsState::OutputColorEncoding, outputColorEncodingOptions);

		ImGui::EndTable();
	}
	ImGui::Dummy(ImVec2(0.0f, 4.0f));
}
