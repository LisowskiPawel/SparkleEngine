#include "PCH.h"

#include "Settings/Rendering/RenderingPtlasSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawPtlasSettingsSection(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings)
{
	static constexpr RenderingSettingsUi::ComboOption<RayTracingPtlasPartitionUpdateMode> partitionUpdateModeOptions[] = {
	    {"Always update partition", RayTracingPtlasPartitionUpdateMode::AlwaysUpdatePartition},
	    {"Always move dynamic to global", RayTracingPtlasPartitionUpdateMode::AlwaysMoveDynamicToGlobal},
	    {"Update partition nearby, move to global otherwise", RayTracingPtlasPartitionUpdateMode::UpdatePartitionNearbyMoveToGlobalOtherwise},
	};

	if (ImGui::TreeNodeEx("PTLAS", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (RenderingSettingsUi::BeginSettingsTable("##RenderingPtlasSettings"))
		{
			bool bPtlasActive = settings.bPtlasActive;
			const bool bPtlasActiveEdited = RenderingSettingsUi::DrawBooleanRow("##PtlasActive", "Enabled", bPtlasActive);
			if (bPtlasActiveEdited)
			{
				settingsController.Set(&EngineRenderingSettingsState::bPtlasActive, bPtlasActive);
			}

			if (settings.bPtlasActive)
			{
				std::uint32_t ptlasPartitionsPerAxis = settings.PtlasPartitionsPerAxis;
				const bool bPtlasPartitionsPerAxisEdited = RenderingSettingsUi::DrawUnsignedIntSliderRow("##PtlasPartitionsPerAxis", "Partitions per axis", ptlasPartitionsPerAxis, 1u, 64u);
				if (bPtlasPartitionsPerAxisEdited)
				{
					settingsController.Set(&EngineRenderingSettingsState::PtlasPartitionsPerAxis, ptlasPartitionsPerAxis);
				}

				RenderingSettingsUi::DrawComboOptionRow(
				    "##PtlasPartitionUpdateMode",
				    "Partition update mode",
				    settings.PtlasPartitionUpdateMode,
				    partitionUpdateModeOptions,
				    [&settingsController](RayTracingPtlasPartitionUpdateMode value) { settingsController.Set(&EngineRenderingSettingsState::PtlasPartitionUpdateMode, value); });

				bool bPtlasMarkAllDynamicInPartition = settings.bPtlasMarkAllDynamicInPartition;

				const bool bPtlasMarkAllDynamicInPartitionEdited = RenderingSettingsUi::DrawBooleanRow(
				    "##PtlasMarkAllDynamicInPartition",
				    "Mark all dynamic in partition",
				    bPtlasMarkAllDynamicInPartition);

				if (bPtlasMarkAllDynamicInPartitionEdited)
				{
					settingsController.Set(&EngineRenderingSettingsState::bPtlasMarkAllDynamicInPartition, bPtlasMarkAllDynamicInPartition);
				}

				float ptlasModeChangeDistance = settings.PtlasModeChangeDistance;
				const bool bPtlasModeChangeDistanceEdited = RenderingSettingsUi::DrawFloatInputRow("##PtlasModeChangeDistance", "Mode change distance", ptlasModeChangeDistance);
				if (bPtlasModeChangeDistanceEdited)
				{
					settingsController.Set(&EngineRenderingSettingsState::PtlasModeChangeDistance, ptlasModeChangeDistance);
				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
