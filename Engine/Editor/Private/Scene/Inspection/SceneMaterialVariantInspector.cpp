#include "PCH.h"

#include "Scene/Inspection/SceneMaterialVariantInspector.h"

#include "Scene/Transactions/EditorTransactionHistory.h"
#include "World/WorldMaterialVariantView.h"
#include "Widgets/UiDetailsWidgets.h"

#include <imgui.h>

namespace SceneMaterialVariantInspector
{
	void Build(const WorldMaterialVariantView& variants, EditorTransactionHistory& transactionHistory, std::uint64_t generation) noexcept
	{
		const std::size_t variantCount = variants.Names.size();
		if (variantCount == 0)
		{
			EditorWidgets::DrawDetailsEmptyState();
			return;
		}

		if (!EditorWidgets::DrawDetailsCategory("Material Variants"))
		{
			return;
		}

		const MaterialVariantIndex activeVariantIndex = variants.Active;
		const char* preview = "Select Variant";
		std::string activeVariantName;
		if (activeVariantIndex < variantCount)
		{
			activeVariantName = variants.Names[activeVariantIndex];
			preview = activeVariantName.c_str();
		}

		ImGui::SetNextItemWidth(-1.0f);
		if (ImGui::BeginCombo("##SceneMaterialVariant", preview))
		{
			for (std::size_t variantIndex = 0; variantIndex < variantCount; ++variantIndex)
			{
				const std::string& variantName = variants.Names[variantIndex];
				const bool selected = variantIndex == activeVariantIndex;
				if (ImGui::Selectable(variantName.c_str(), selected))
				{
					const MaterialVariantIndex selected = static_cast<MaterialVariantIndex>(variantIndex);
					if (activeVariantIndex != kInvalidMaterialVariantIndex)
					{
						(void) transactionHistory.Execute({0, SetMaterialVariantCommand{selected}}, {0, SetMaterialVariantCommand{activeVariantIndex}}, generation);
					}
					else
					{
						(void) transactionHistory.Execute({0, SetMaterialVariantCommand{selected}}, {0, SetMaterialVariantCommand{selected}}, generation);
					}
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}
