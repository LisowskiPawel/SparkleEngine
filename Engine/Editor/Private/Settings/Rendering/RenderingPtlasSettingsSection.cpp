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
			bool ptlasActive = settings.PtlasActive;
			const bool ptlasActiveEdited = RenderingSettingsUi::DrawBooleanRow("##PtlasActive", "Enabled", ptlasActive);
			if (ptlasActiveEdited)
			{
				settingsController.SetPtlasActive(ptlasActive);
			}

			if (settings.PtlasActive)
			{
				std::uint32_t ptlasPartitionsPerAxis = settings.PtlasPartitionsPerAxis;
				const bool ptlasPartitionsPerAxisEdited = RenderingSettingsUi::DrawUnsignedIntSliderRow("##PtlasPartitionsPerAxis", "Partitions per axis", ptlasPartitionsPerAxis, 1u, 64u);
				if (ptlasPartitionsPerAxisEdited)
				{
					settingsController.SetPtlasPartitionsPerAxis(ptlasPartitionsPerAxis);
				}

				RenderingSettingsUi::DrawComboOptionRow(
				    "##PtlasPartitionUpdateMode",
				    "Partition update mode",
				    settings.PtlasPartitionUpdateMode,
				    partitionUpdateModeOptions,
				    [&settingsController](RayTracingPtlasPartitionUpdateMode value) { settingsController.SetPtlasPartitionUpdateMode(value); });

				bool ptlasMarkAllDynamicInPartition = settings.PtlasMarkAllDynamicInPartition;

				const bool ptlasMarkAllDynamicInPartitionEdited = RenderingSettingsUi::DrawBooleanRow(
				    "##PtlasMarkAllDynamicInPartition",
				    "Mark all dynamic in partition",
				    ptlasMarkAllDynamicInPartition);

				if (ptlasMarkAllDynamicInPartitionEdited)
				{
					settingsController.SetPtlasMarkAllDynamicInPartition(ptlasMarkAllDynamicInPartition);
				}

				float ptlasModeChangeDistance = settings.PtlasModeChangeDistance;
				const bool ptlasModeChangeDistanceEdited = RenderingSettingsUi::DrawFloatInputRow("##PtlasModeChangeDistance", "Mode change distance", ptlasModeChangeDistance);
				if (ptlasModeChangeDistanceEdited)
				{
					settingsController.SetPtlasModeChangeDistance(ptlasModeChangeDistance);
				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
