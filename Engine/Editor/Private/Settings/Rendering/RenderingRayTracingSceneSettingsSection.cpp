#include "PCH.h"

#include "Settings/Rendering/RenderingRayTracingSceneSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Settings/Rendering/RenderingPtlasSettingsSection.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawRayTracingSceneSettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	const bool bRayTracingSceneMatches = RenderingSettingsUi::MatchesFilter(
	    filterText,
	    "Ray Tracing Scene",
	    "ray tracing scene tlas refit ptlas active partition update mode partitions dynamic distance acceleration structure");

	const bool bRayTracingSceneOpen = bRayTracingSceneMatches && RenderingSettingsUi::BeginSettingsCategory("Ray Tracing Scene");

	if (!bRayTracingSceneOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingRayTracingSceneSettings"))
	{
		ImGui::BeginDisabled(settings.bPtlasActive);
		bool bRefitTlas = settings.bRefitTlas;
		const bool bRefitTlasEdited = RenderingSettingsUi::DrawBooleanRow("##RefitTlas", "Refit TLAS", bRefitTlas);
		if (bRefitTlasEdited)
		{
			settingsController.SetRefitTlas(bRefitTlas);
		}

		ImGui::EndDisabled();
		ImGui::EndTable();
	}

	DrawPtlasSettingsSection(settingsController, settings);
}
