#include "PCH.h"

#include "Settings/Rendering/RenderingGeometrySettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawGeometrySettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings, const char* filterText)
{
	static constexpr RenderingSettingsUi::ComboOption<GBufferAlgorithm> gBufferAlgorithmOptions[] = {{"Rasterized", GBufferAlgorithm::Rasterized}, {"Ray tracing", GBufferAlgorithm::RayTracing}};
	const bool bGeometryMatches = RenderingSettingsUi::MatchesFilter(filterText, "Geometry", "geometry mesh auto batching");
	const bool bGeometryOpen = bGeometryMatches && RenderingSettingsUi::BeginSettingsCategory("Geometry");

	if (bGeometryOpen)
	{
		if (RenderingSettingsUi::BeginSettingsTable("##RenderingGeometrySettings"))
		{
			RenderingSettingsUi::DrawComboOptionRow(
			    "##GBufferAlgorithm",
			    "GBuffer algorithm",
			    settings.SelectedGBufferAlgorithm,
			    gBufferAlgorithmOptions,
			    [&settingsController](GBufferAlgorithm value) { settingsController.SetGBufferAlgorithm(value); });

			bool bMeshAutoBatching = settings.bMeshAutoBatching;
			const bool bMeshAutoBatchingEdited = RenderingSettingsUi::DrawBooleanRow("##MeshAutoBatching", "Mesh auto batching", bMeshAutoBatching);
			if (bMeshAutoBatchingEdited)
			{
				settingsController.SetMeshAutoBatching(bMeshAutoBatching);
			}

			ImGui::EndTable();
		}
		ImGui::Dummy(ImVec2(0.0f, 4.0f));
	}
}
