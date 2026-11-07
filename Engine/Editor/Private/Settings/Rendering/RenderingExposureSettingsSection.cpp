#include "PCH.h"

#include "Settings/Rendering/RenderingExposureSettingsSection.h"

#include "Settings/Rendering/ExposureSettingsEditor.h"
#include "Settings/Rendering/RenderingSettingsUi.h"

#include <imgui.h>

void DrawExposureSettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	const bool bExposureMatches = RenderingSettingsUi::MatchesFilter(
	    filterText,
	    "Exposure",
	    "exposure automatic manual metering reduction downsample pyramid compensation ev luminance min max adapt speed");

	const bool bExposureOpen = bExposureMatches && RenderingSettingsUi::BeginSettingsCategory("Exposure");

	if (!bExposureOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingExposureSettings"))
	{
		ExposureSettingsEditor::DrawSettings(settingsController, settings);
		ImGui::EndTable();
	}
}
