#include "PCH.h"

#include "Settings/Rendering/RenderingExposureSettingsSection.h"

#include "Settings/Rendering/ExposureSettingsEditor.h"
#include "Settings/Rendering/RenderingSettingsUi.h"

#include <imgui.h>

void DrawExposureSettingsSection(EngineRenderingSettingsController& settingsController, const char* filterText)
{
	const bool bExposureOpen = RenderingSettingsUi::BeginSettingsCategory(filterText, "Exposure", "exposure automatic manual metering reduction downsample pyramid compensation ev luminance min max adapt speed");

	if (!bExposureOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingExposureSettings"))
	{
		ExposureSettingsEditor::DrawSettings(settingsController);
		ImGui::EndTable();
	}
}
