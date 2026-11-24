#include "PCH.h"

#include "Settings/Rendering/ExposureSettingsEditor.h"

#include "Settings/Rendering/RenderingSettingsUi.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "Settings/EngineRenderingSettingsController.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

#include <imgui.h>

#include <cfloat>
#include <optional>

namespace ExposureSettingsEditor
{
	template <typename TValue> struct ExposureOverrideBinding final
	{
		bool& bEnabled;
		TValue& Value;
		TValue DefaultValue;
	};

	constexpr RenderingSettingsUi::ComboOption<EngineExposureMode> ExposureModeOptions[] = {
	    {"Manual", EngineExposureMode::Manual},
	    {"Automatic", EngineExposureMode::Automatic},
	};

	constexpr RenderingSettingsUi::ComboOption<EngineExposureMeteringMethod> ExposureMeteringOptions[] = {
	    {"Histogram", EngineExposureMeteringMethod::Histogram},
	    {"Downsample pyramid", EngineExposureMeteringMethod::DownsamplePyramid},
	};

	struct ExposureFloatOverride final
	{
		const char* Id;
		const char* Label;
		bool ViewportExposureOverrides::* Enabled;
		float ViewportExposureOverrides::* Value;
		float EngineRenderingSettingsState::* DefaultValue;
		float Speed;
		float Minimum;
		float Maximum;
		const char* Format;
		std::optional<EngineExposureMode> RequiredMode;
	};

	constexpr ExposureFloatOverride FloatOverrides[] = {
	    {"Manual",
	        "Manual exposure",
	        &ViewportExposureOverrides::bOverrideManualExposure,
	        &ViewportExposureOverrides::ManualExposure,
	        &EngineRenderingSettingsState::ManualExposure,
	        0.05f,
	        0.0f,
	        65536.0f,
	        "%.4f",
	        EngineExposureMode::Manual},
	    {"Compensation",
	        "Compensation (EV)",
	        &ViewportExposureOverrides::bOverrideCompensation,
	        &ViewportExposureOverrides::Compensation,
	        &EngineRenderingSettingsState::ExposureCompensation,
	        0.1f,
	        -16.0f,
	        16.0f,
	        "%.2f",
	        std::nullopt},
	    {"Target",
	        "Target luminance",
	        &ViewportExposureOverrides::bOverrideTargetLuminance,
	        &ViewportExposureOverrides::TargetLuminance,
	        &EngineRenderingSettingsState::ExposureTargetLuminance,
	        0.01f,
	        0.0001f,
	        16.0f,
	        "%.4f",
	        EngineExposureMode::Automatic},
	    {"Minimum",
	        "Minimum exposure",
	        &ViewportExposureOverrides::bOverrideMinimum,
	        &ViewportExposureOverrides::Minimum,
	        &EngineRenderingSettingsState::ExposureMin,
	        0.001f,
	        0.0f,
	        65536.0f,
	        "%.6f",
	        EngineExposureMode::Automatic},
	    {"Maximum",
	        "Maximum exposure",
	        &ViewportExposureOverrides::bOverrideMaximum,
	        &ViewportExposureOverrides::Maximum,
	        &EngineRenderingSettingsState::ExposureMax,
	        1.0f,
	        0.0f,
	        65536.0f,
	        "%.3f",
	        EngineExposureMode::Automatic},
	    {"AdaptUp",
	        "Adapt rate up (1/s)",
	        &ViewportExposureOverrides::bOverrideAdaptationSpeedUp,
	        &ViewportExposureOverrides::AdaptationSpeedUp,
	        &EngineRenderingSettingsState::ExposureAdaptationSpeedUp,
	        0.1f,
	        0.0f,
	        100.0f,
	        "%.3f",
	        EngineExposureMode::Automatic},
	    {"AdaptDown",
	        "Adapt rate down (1/s)",
	        &ViewportExposureOverrides::bOverrideAdaptationSpeedDown,
	        &ViewportExposureOverrides::AdaptationSpeedDown,
	        &EngineRenderingSettingsState::ExposureAdaptationSpeedDown,
	        0.1f,
	        0.0f,
	        100.0f,
	        "%.3f",
	        EngineExposureMode::Automatic},
	};

	template <typename TValue> void BeginOverrideRow(const char* label, ExposureOverrideBinding<TValue> binding, bool& bChanged) noexcept
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		if (ImGui::Checkbox("##Override", &binding.bEnabled))
		{
			if (binding.bEnabled)
			{
				binding.Value = binding.DefaultValue;
			}
			bChanged = true;
		}
		ImGui::TableSetColumnIndex(1);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::TableSetColumnIndex(2);
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	bool BeginOverrideTable() noexcept
	{
		if (!ImGui::BeginTable("##ViewportExposure", 3, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH))
		{
			return false;
		}
		ImGui::TableSetupColumn("Override", ImGuiTableColumnFlags_WidthFixed, 28.0f);
		ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 175.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
		return true;
	}

	template <typename TEnum, std::size_t N>
	bool DrawEnumOverride(const char* id, const char* label, const RenderingSettingsUi::ComboOption<TEnum> (&options)[N], ExposureOverrideBinding<TEnum> binding) noexcept
	{
		bool bChanged = false;
		ImGui::PushID(id);
		BeginOverrideRow(label, binding, bChanged);
		ImGui::BeginDisabled(!binding.bEnabled);
		const char* preview = "Unknown";
		for (const RenderingSettingsUi::ComboOption<TEnum>& option : options)
		{
			if (binding.Value == option.Value)
			{
				preview = option.Label;
				break;
			}
		}
		if (ImGui::BeginCombo("##Value", preview))
		{
			for (const RenderingSettingsUi::ComboOption<TEnum>& option : options)
			{
				if (ImGui::Selectable(option.Label, binding.Value == option.Value))
				{
					binding.Value = option.Value;
					bChanged = true;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::EndDisabled();
		ImGui::PopID();
		return bChanged;
	}

	bool DrawFloatOverride(const ExposureFloatOverride& property, ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults, EngineExposureMode effectiveMode) noexcept
	{
		ExposureOverrideBinding<float> binding{exposure.*property.Enabled, exposure.*property.Value, defaults.*property.DefaultValue};
		bool bChanged = false;
		ImGui::PushID(property.Id);
		BeginOverrideRow(property.Label, binding, bChanged);
		const bool bValueEnabled = !property.RequiredMode || effectiveMode == *property.RequiredMode;
		ImGui::BeginDisabled(!binding.bEnabled || !bValueEnabled);
		bChanged |= ImGui::DragFloat("##Value", &binding.Value, property.Speed, property.Minimum, property.Maximum, property.Format);
		ImGui::EndDisabled();
		ImGui::PopID();
		return bChanged;
	}

	void DrawDefaultExposureModes(EngineRenderingSettingsController& settingsController)
	{
		RenderingSettingsUi::DrawComboOptionRow("##ExposureMode", "Exposure mode", settingsController, &EngineRenderingSettingsState::ExposureMode, ExposureModeOptions);

		RenderingSettingsUi::DrawComboOptionRow("##ExposureMeteringMethod", "Exposure metering", settingsController, &EngineRenderingSettingsState::ExposureMeteringMethod, ExposureMeteringOptions);
	}

	void DrawDefaultExposureValues(EngineRenderingSettingsController& settingsController)
	{
		const auto& settings = settingsController.GetState();
		ImGui::BeginDisabled(settings.ExposureMode != EngineExposureMode::Manual);

		RenderingSettingsUi::DrawFloatInputRow("##ManualExposure", "Manual exposure", settingsController, &EngineRenderingSettingsState::ManualExposure, 0.1f, 1.0f, "%.4f");

		ImGui::EndDisabled();

		RenderingSettingsUi::DrawFloatInputRow("##ExposureCompensation", "Exposure compensation EV", settingsController, &EngineRenderingSettingsState::ExposureCompensation, 0.1f, 1.0f, "%.2f");

		RenderingSettingsUi::DrawFloatInputRow("##ExposureTargetLuminance", "Target luminance", settingsController, &EngineRenderingSettingsState::ExposureTargetLuminance, 0.01f, 0.1f, "%.4f");

		RenderingSettingsUi::DrawFloatInputRow("##ExposureMin", "Min exposure", settingsController, &EngineRenderingSettingsState::ExposureMin, 0.0001f, 0.01f, "%.6f");

		RenderingSettingsUi::DrawFloatInputRow("##ExposureMax", "Max exposure", settingsController, &EngineRenderingSettingsState::ExposureMax, 1.0f, 64.0f, "%.3f");

		RenderingSettingsUi::DrawFloatInputRow("##ExposureAdaptationSpeedUp", "Adapt rate up (1/s)", settingsController, &EngineRenderingSettingsState::ExposureAdaptationSpeedUp, 0.1f, 1.0f, "%.3f");

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ExposureAdaptationSpeedDown",
		    "Adapt rate down (1/s)",
		    settingsController,
		    &EngineRenderingSettingsState::ExposureAdaptationSpeedDown,
		    0.1f,
		    1.0f,
		    "%.3f");
	}

	bool DrawOverrideModes(ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults) noexcept
	{
		bool bChanged = DrawEnumOverride("Mode", "Mode", ExposureModeOptions, ExposureOverrideBinding<EngineExposureMode>{exposure.bOverrideMode, exposure.Mode, defaults.ExposureMode});

		bChanged |= DrawEnumOverride(
		    "Metering",
		    "Metering",
		    ExposureMeteringOptions,
		    ExposureOverrideBinding<EngineExposureMeteringMethod>{exposure.bOverrideMeteringMethod, exposure.MeteringMethod, defaults.ExposureMeteringMethod});

		return bChanged;
	}

	bool DrawOverrideValues(ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults) noexcept
	{
		const EngineExposureMode effectiveMode = exposure.bOverrideMode ? exposure.Mode : defaults.ExposureMode;
		bool bChanged = false;
		for (const ExposureFloatOverride& property : FloatOverrides)
		{
			bChanged |= DrawFloatOverride(property, exposure, defaults, effectiveMode);
		}
		return bChanged;
	}
}

void ExposureSettingsEditor::DrawSettings(EngineRenderingSettingsController& settingsController)
{
	DrawDefaultExposureModes(settingsController);
	DrawDefaultExposureValues(settingsController);
}

bool ExposureSettingsEditor::DrawOverrides(ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults) noexcept
{
	if (!BeginOverrideTable())
	{
		return false;
	}
	bool bChanged = DrawOverrideModes(exposure, defaults);
	bChanged |= DrawOverrideValues(exposure, defaults);
	ImGui::EndTable();
	return bChanged;
}
