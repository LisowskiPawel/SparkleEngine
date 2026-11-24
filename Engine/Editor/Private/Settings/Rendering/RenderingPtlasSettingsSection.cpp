#include "PCH.h"

#include "Settings/Rendering/RenderingPtlasSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawPtlasSettingsSection(EngineRenderingSettingsController& settingsController)
{
	const auto& settings = settingsController.GetState();
	static constexpr RenderingSettingsUi::ComboOption<RayTracingPtlasPartitionUpdateMode> partitionUpdateModeOptions[] = {
	    {"Always update partition", RayTracingPtlasPartitionUpdateMode::AlwaysUpdatePartition},
	    {"Always move dynamic to global", RayTracingPtlasPartitionUpdateMode::AlwaysMoveDynamicToGlobal},
	    {"Update partition nearby, move to global otherwise", RayTracingPtlasPartitionUpdateMode::UpdatePartitionNearbyMoveToGlobalOtherwise},
	};

	if (ImGui::TreeNodeEx("PTLAS", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (RenderingSettingsUi::BeginSettingsTable("##RenderingPtlasSettings"))
		{
			RenderingSettingsUi::DrawBooleanRow("##PtlasActive", "Enabled", settingsController, &EngineRenderingSettingsState::bPtlasActive);

			if (settings.bPtlasActive)
			{
				RenderingSettingsUi::DrawUnsignedIntSliderRow("##PtlasPartitionsPerAxis", "Partitions per axis", settingsController, &EngineRenderingSettingsState::PtlasPartitionsPerAxis, 1u, 64u);

				RenderingSettingsUi::DrawComboOptionRow(
				    "##PtlasPartitionUpdateMode",
				    "Partition update mode",
				    settingsController,
				    &EngineRenderingSettingsState::PtlasPartitionUpdateMode,
				    partitionUpdateModeOptions);

				RenderingSettingsUi::DrawBooleanRow(
				    "##PtlasMarkAllDynamicInPartition",
				    "Mark all dynamic in partition",
				    settingsController,
				    &EngineRenderingSettingsState::bPtlasMarkAllDynamicInPartition);

				RenderingSettingsUi::DrawFloatInputRow("##PtlasModeChangeDistance", "Mode change distance", settingsController, &EngineRenderingSettingsState::PtlasModeChangeDistance);
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
