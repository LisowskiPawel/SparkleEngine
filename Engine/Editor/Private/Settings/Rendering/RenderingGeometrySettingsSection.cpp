#include "PCH.h"

#include "Settings/Rendering/RenderingGeometrySettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawGeometrySettingsSection(EngineRenderingSettingsController& settingsController, const char* filterText)
{
	static constexpr RenderingSettingsUi::ComboOption<GBufferAlgorithm> gBufferAlgorithmOptions[] = {{"Rasterized", GBufferAlgorithm::Rasterized}, {"Ray tracing", GBufferAlgorithm::RayTracing}};
	const bool bGeometryOpen = RenderingSettingsUi::BeginSettingsCategory(filterText, "Geometry", "geometry mesh auto batching");

	if (bGeometryOpen)
	{
		if (RenderingSettingsUi::BeginSettingsTable("##RenderingGeometrySettings"))
		{
			RenderingSettingsUi::DrawComboOptionRow("##GBufferAlgorithm", "GBuffer algorithm", settingsController, &EngineRenderingSettingsState::SelectedGBufferAlgorithm, gBufferAlgorithmOptions);

			RenderingSettingsUi::DrawBooleanRow("##MeshAutoBatching", "Mesh auto batching", settingsController, &EngineRenderingSettingsState::bMeshAutoBatching);

			ImGui::EndTable();
		}
		ImGui::Dummy(ImVec2(0.0f, 4.0f));
	}
}
