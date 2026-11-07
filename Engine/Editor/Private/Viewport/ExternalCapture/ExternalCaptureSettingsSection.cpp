#include "PCH.h"
#include "Viewport/ExternalCapture/ExternalCaptureSettingsSection.h"
#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

static void DrawStartupCaptureSelection(EngineRenderingSettingsController& settings)
{
	const auto provider = settings.GetState().StartupCaptureProvider;
	const auto label = ExternalCaptureProviderDisplayName(provider);
	if (ImGui::BeginCombo("Capture tool on startup", label.data()))
	{
		for (auto candidate : ExternalCaptureProviders)
		{
			if (ImGui::Selectable(ExternalCaptureProviderDisplayName(candidate).data(), candidate == provider))
			{
				settings.SetStartupCaptureProvider(candidate);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Attach this capture tool when the Editor starts");
	}
}

void DrawExternalCaptureSettingsSection(EngineRenderingSettingsController& settings, const char* filterText)
{
	const auto& state = settings.GetState();
	const bool captureAvailable = state.CaptureToolsInstalled || state.StartupCaptureProvider != ExternalCaptureProvider::None;
	const bool gpuCaptureMatches = captureAvailable && RenderingSettingsUi::MatchesFilter(filterText, "GPU Capture", "pix nsight renderdoc capture attach startup");
	const bool gpuCaptureOpen = gpuCaptureMatches && RenderingSettingsUi::BeginSettingsCategory("GPU Capture");

	if (!gpuCaptureOpen)
	{
		return;
	}

	DrawStartupCaptureSelection(settings);
	ImGui::TextDisabled("Restart Editor to apply.");
	ImGui::Dummy(ImVec2(0.0f, 4.0f));
}
