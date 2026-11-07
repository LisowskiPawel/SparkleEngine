#include "PCH.h"

#include "Settings/Rendering/RenderingGeometrySettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawGeometrySettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	static constexpr RenderingSettingsUi::ComboOption<GBufferAlgorithm> gBufferAlgorithmOptions[] = {{"Rasterized", GBufferAlgorithm::Rasterized}, {"Ray tracing", GBufferAlgorithm::RayTracing}};
	const bool geometryMatches = RenderingSettingsUi::MatchesFilter(filterText, "Geometry", "geometry mesh auto batching");
	const bool geometryOpen = geometryMatches && RenderingSettingsUi::BeginSettingsCategory("Geometry");

	if (geometryOpen)
	{
		if (RenderingSettingsUi::BeginSettingsTable("##RenderingGeometrySettings"))
		{
			RenderingSettingsUi::DrawComboOptionRow(
			    "##GBufferAlgorithm",
			    "GBuffer algorithm",
			    settings.SelectedGBufferAlgorithm,
			    gBufferAlgorithmOptions,
			    [&settingsController](GBufferAlgorithm value) { settingsController.SetGBufferAlgorithm(value); });

			bool meshAutoBatching = settings.MeshAutoBatching;
			const bool meshAutoBatchingEdited = RenderingSettingsUi::DrawBooleanRow("##MeshAutoBatching", "Mesh auto batching", meshAutoBatching);
			if (meshAutoBatchingEdited)
			{
				settingsController.SetMeshAutoBatching(meshAutoBatching);
			}

			ImGui::EndTable();
		}
		ImGui::Dummy(ImVec2(0.0f, 4.0f));
	}
}
