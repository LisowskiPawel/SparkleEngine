#include "PCH.h"

#include "Settings/Rendering/RenderingRayReconstructionSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawRayReconstructionSettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	static constexpr RenderingSettingsUi::ComboOption<EngineRayReconstructionMode> rayReconstructionModeOptions[] = {
	    {"Off", EngineRayReconstructionMode::Off},
	    {"NVIDIA DLSS Ray Reconstruction", EngineRayReconstructionMode::NvidiaDlssRayReconstruction},
	};

	const bool rayReconstructionMatches = RenderingSettingsUi::MatchesFilter(filterText, "Ray Reconstruction", "dlss ray reconstruction indirect specular diffuse");
	const bool rayReconstructionOpen = rayReconstructionMatches && RenderingSettingsUi::BeginSettingsCategory("Ray Reconstruction");
	
	if (!rayReconstructionOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingRayReconstructionSettings"))
	{
		RenderingSettingsUi::DrawComboOptionRow(
		    "##RayReconstructionMode",
		    "Mode",
		    settings.RayReconstructionMode,
		    rayReconstructionModeOptions,
		    [&settingsController](EngineRayReconstructionMode value) { settingsController.SetRayReconstructionMode(value); });

		ImGui::EndTable();
	}
	ImGui::Dummy(ImVec2(0.0f, 4.0f));
}
