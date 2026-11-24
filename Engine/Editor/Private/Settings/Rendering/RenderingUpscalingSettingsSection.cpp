#include "PCH.h"

#include "Settings/Rendering/RenderingUpscalingSettingsSection.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

void DrawUpscalingSettingsSection(EngineRenderingSettingsController& settingsController, const char* filterText)
{
	static constexpr RenderingSettingsUi::ComboOption<EUpscalerProviderKind> upscalerProviderOptions[] = {
	    {"Linear", EUpscalerProviderKind::Linear},
	    {"NVIDIA DLSS", EUpscalerProviderKind::NvidiaDlss},
	};

	static constexpr RenderingSettingsUi::ComboOption<EUpscalerQualityMode> upscalerQualityOptions[] = {
	    {"Native AA", EUpscalerQualityMode::NativeAA},
	    {"Quality", EUpscalerQualityMode::Quality},
	    {"Balanced", EUpscalerQualityMode::Balanced},
	    {"Performance", EUpscalerQualityMode::Performance},
	    {"Ultra performance", EUpscalerQualityMode::UltraPerformance},
	};

	const bool bUpscalingOpen = RenderingSettingsUi::BeginSettingsCategory(filterText, "Upscaling", "upscaler upscaling linear bilinear dlss quality native aa balanced performance");

	if (!bUpscalingOpen)
	{
		return;
	}

	if (RenderingSettingsUi::BeginSettingsTable("##RenderingUpscalingSettings"))
	{
		RenderingSettingsUi::DrawComboOptionRow("##UpscalerProvider", "Provider", settingsController, &EngineRenderingSettingsState::UpscalerProvider, upscalerProviderOptions);

		RenderingSettingsUi::DrawComboOptionRow("##UpscalerQualityMode", "Quality mode", settingsController, &EngineRenderingSettingsState::UpscalerQualityMode, upscalerQualityOptions);

		ImGui::EndTable();
	}
	ImGui::Dummy(ImVec2(0.0f, 4.0f));
}
