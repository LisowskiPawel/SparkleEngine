#include "PCH.h"

#include "Settings/Rendering/RenderingRayTracingSceneSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Settings/Rendering/RenderingPtlasSettingsSection.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawRayTracingSceneSettingsSection(EngineRenderingSettingsController& settingsController, const char* filterText)
{
	const auto& settings = settingsController.GetState();
	const bool bRayTracingSceneOpen = RenderingSettingsUi::BeginSettingsCategory(
	    filterText,
	    "Ray Tracing Scene",
	    "ray tracing scene tlas refit ptlas active partition update mode partitions dynamic distance acceleration structure");

	if (!bRayTracingSceneOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingRayTracingSceneSettings"))
	{
		ImGui::BeginDisabled(settings.bPtlasActive);
		RenderingSettingsUi::DrawBooleanRow("##RefitTlas", "Refit TLAS", settingsController, &EngineRenderingSettingsState::bRefitTlas);

		ImGui::EndDisabled();
		ImGui::EndTable();
	}

	DrawPtlasSettingsSection(settingsController);
}
