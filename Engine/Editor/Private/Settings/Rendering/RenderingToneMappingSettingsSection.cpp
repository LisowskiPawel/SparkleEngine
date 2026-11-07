#include "PCH.h"

#include "Settings/Rendering/RenderingToneMappingSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawToneMappingSettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	static constexpr RenderingSettingsUi::ComboOption<EngineToneMapper> toneMapperOptions[] = {
	    {"Reinhard", EngineToneMapper::Reinhard},
	    {"ACES approximate", EngineToneMapper::AcesApprox},
	    {"ACES fitted filmic", EngineToneMapper::AcesFilmic},
	};

	const bool bToneMappingMatches = RenderingSettingsUi::MatchesFilter(filterText, "Tone Mapping", "tone mapper aces reinhard filmic");
	const bool bToneMappingOpen = bToneMappingMatches && RenderingSettingsUi::BeginSettingsCategory("Tone Mapping");

	if (!bToneMappingOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingToneMappingSettings"))
	{
		RenderingSettingsUi::DrawComboOptionRow(
		    "##ToneMapper",
		    "Tone mapper",
		    settings.ToneMapper,
		    toneMapperOptions,
		    [&settingsController](EngineToneMapper value) { settingsController.SetToneMapper(value); });

		ImGui::EndTable();
	}
}
