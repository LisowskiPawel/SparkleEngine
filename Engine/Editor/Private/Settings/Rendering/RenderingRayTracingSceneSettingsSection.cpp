#include "PCH.h"

#include "Settings/Rendering/RenderingRayTracingSceneSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Settings/Rendering/RenderingPtlasSettingsSection.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawRayTracingSceneSettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	const bool rayTracingSceneMatches = RenderingSettingsUi::MatchesFilter(filterText, "Ray Tracing Scene", "ray tracing scene tlas refit ptlas active partition update mode partitions dynamic distance acceleration structure");
	const bool rayTracingSceneOpen = rayTracingSceneMatches && RenderingSettingsUi::BeginSettingsCategory("Ray Tracing Scene");
	
	if (!rayTracingSceneOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingRayTracingSceneSettings"))
	{
		ImGui::BeginDisabled(settings.PtlasActive);
		RenderingSettingsUi::DrawBooleanRow("##RefitTlas", "Refit TLAS", settings.RefitTlas, [&settingsController](bool value) { settingsController.SetRefitTlas(value); });
		ImGui::EndDisabled();
		ImGui::EndTable();
	}

	DrawPtlasSettingsSection(settingsController, settings);
}
