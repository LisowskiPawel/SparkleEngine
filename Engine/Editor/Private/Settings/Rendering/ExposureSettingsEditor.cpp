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
		bool& Enabled;
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
	        &ViewportExposureOverrides::OverrideManualExposure,
	        &ViewportExposureOverrides::ManualExposure,
	        &EngineRenderingSettingsState::ManualExposure,
	        0.05f,
	        0.0f,
	        65536.0f,
	        "%.4f",
	        EngineExposureMode::Manual},
	    {"Compensation",
	        "Compensation (EV)",
	        &ViewportExposureOverrides::OverrideCompensation,
	        &ViewportExposureOverrides::Compensation,
	        &EngineRenderingSettingsState::ExposureCompensation,
	        0.1f,
	        -16.0f,
	        16.0f,
	        "%.2f",
	        std::nullopt},
	    {"Target",
	        "Target luminance",
	        &ViewportExposureOverrides::OverrideTargetLuminance,
	        &ViewportExposureOverrides::TargetLuminance,
	        &EngineRenderingSettingsState::ExposureTargetLuminance,
	        0.01f,
	        0.0001f,
	        16.0f,
	        "%.4f",
	        EngineExposureMode::Automatic},
	    {"Minimum",
	        "Minimum exposure",
	        &ViewportExposureOverrides::OverrideMinimum,
	        &ViewportExposureOverrides::Minimum,
	        &EngineRenderingSettingsState::ExposureMin,
	        0.001f,
	        0.0f,
	        65536.0f,
	        "%.6f",
	        EngineExposureMode::Automatic},
	    {"Maximum",
	        "Maximum exposure",
	        &ViewportExposureOverrides::OverrideMaximum,
	        &ViewportExposureOverrides::Maximum,
	        &EngineRenderingSettingsState::ExposureMax,
	        1.0f,
	        0.0f,
	        65536.0f,
	        "%.3f",
	        EngineExposureMode::Automatic},
	    {"AdaptUp",
	        "Adapt rate up (1/s)",
	        &ViewportExposureOverrides::OverrideAdaptationSpeedUp,
	        &ViewportExposureOverrides::AdaptationSpeedUp,
	        &EngineRenderingSettingsState::ExposureAdaptationSpeedUp,
	        0.1f,
	        0.0f,
	        100.0f,
	        "%.3f",
	        EngineExposureMode::Automatic},
	    {"AdaptDown",
	        "Adapt rate down (1/s)",
	        &ViewportExposureOverrides::OverrideAdaptationSpeedDown,
	        &ViewportExposureOverrides::AdaptationSpeedDown,
	        &EngineRenderingSettingsState::ExposureAdaptationSpeedDown,
	        0.1f,
	        0.0f,
	        100.0f,
	        "%.3f",
	        EngineExposureMode::Automatic},
	};

	template <typename TValue> void BeginOverrideRow(const char* label, ExposureOverrideBinding<TValue> binding, bool& changed) noexcept
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		if (ImGui::Checkbox("##Override", &binding.Enabled))
		{
			if (binding.Enabled)
			{
				binding.Value = binding.DefaultValue;
			}
			changed = true;
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
		bool changed = false;
		ImGui::PushID(id);
		BeginOverrideRow(label, binding, changed);
		ImGui::BeginDisabled(!binding.Enabled);
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
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::EndDisabled();
		ImGui::PopID();
		return changed;
	}

	bool DrawFloatOverride(const ExposureFloatOverride& property, ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults, EngineExposureMode effectiveMode) noexcept
	{
		ExposureOverrideBinding<float> binding{exposure.*property.Enabled, exposure.*property.Value, defaults.*property.DefaultValue};
		bool changed = false;
		ImGui::PushID(property.Id);
		BeginOverrideRow(property.Label, binding, changed);
		const bool valueEnabled = !property.RequiredMode || effectiveMode == *property.RequiredMode;
		ImGui::BeginDisabled(!binding.Enabled || !valueEnabled);
		changed |= ImGui::DragFloat("##Value", &binding.Value, property.Speed, property.Minimum, property.Maximum, property.Format);
		ImGui::EndDisabled();
		ImGui::PopID();
		return changed;
	}

	void DrawDefaultExposureModes(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings)
	{
		RenderingSettingsUi::DrawComboOptionRow(
		    "##ExposureMode",
		    "Exposure mode",
		    settings.ExposureMode,
		    ExposureModeOptions,
		    [&settingsController](EngineExposureMode value) { settingsController.SetExposureMode(value); });

		RenderingSettingsUi::DrawComboOptionRow(
		    "##ExposureMeteringMethod",
		    "Exposure metering",
		    settings.ExposureMeteringMethod,
		    ExposureMeteringOptions,
		    [&settingsController](EngineExposureMeteringMethod value) { settingsController.SetExposureMeteringMethod(value); });
	}

	void DrawDefaultExposureValues(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings)
	{
		ImGui::BeginDisabled(settings.ExposureMode != EngineExposureMode::Manual);

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ManualExposure",
		    "Manual exposure",
		    settings.ManualExposure,
		    [&settingsController](float value) { settingsController.SetManualExposure(value); },
		    0.1f,
		    1.0f,
		    "%.4f");

		ImGui::EndDisabled();

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ExposureCompensation",
		    "Exposure compensation EV",
		    settings.ExposureCompensation,
		    [&settingsController](float value) { settingsController.SetExposureCompensation(value); },
		    0.1f,
		    1.0f,
		    "%.2f");

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ExposureTargetLuminance",
		    "Target luminance",
		    settings.ExposureTargetLuminance,
		    [&settingsController](float value) { settingsController.SetExposureTargetLuminance(value); },
		    0.01f,
		    0.1f,
		    "%.4f");

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ExposureMin",
		    "Min exposure",
		    settings.ExposureMin,
		    [&settingsController](float value) { settingsController.SetExposureMin(value); },
		    0.0001f,
		    0.01f,
		    "%.6f");

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ExposureMax",
		    "Max exposure",
		    settings.ExposureMax,
		    [&settingsController](float value) { settingsController.SetExposureMax(value); },
		    1.0f,
		    64.0f,
		    "%.3f");

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ExposureAdaptationSpeedUp",
		    "Adapt rate up (1/s)",
		    settings.ExposureAdaptationSpeedUp,
		    [&settingsController](float value) { settingsController.SetExposureAdaptationSpeedUp(value); },
		    0.1f,
		    1.0f,
		    "%.3f");

		RenderingSettingsUi::DrawFloatInputRow(
		    "##ExposureAdaptationSpeedDown",
		    "Adapt rate down (1/s)",
		    settings.ExposureAdaptationSpeedDown,
		    [&settingsController](float value) { settingsController.SetExposureAdaptationSpeedDown(value); },
		    0.1f,
		    1.0f,
		    "%.3f");
	}

	bool DrawOverrideModes(ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults) noexcept
	{
		bool changed = DrawEnumOverride("Mode", "Mode", ExposureModeOptions, ExposureOverrideBinding<EngineExposureMode>{exposure.OverrideMode, exposure.Mode, defaults.ExposureMode});

		changed |= DrawEnumOverride(
		    "Metering",
		    "Metering",
		    ExposureMeteringOptions,
		    ExposureOverrideBinding<EngineExposureMeteringMethod>{exposure.OverrideMeteringMethod, exposure.MeteringMethod, defaults.ExposureMeteringMethod});

		return changed;
	}

	bool DrawOverrideValues(ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults) noexcept
	{
		const EngineExposureMode effectiveMode = exposure.OverrideMode ? exposure.Mode : defaults.ExposureMode;
		bool changed = false;
		for (const ExposureFloatOverride& property : FloatOverrides)
		{
			changed |= DrawFloatOverride(property, exposure, defaults, effectiveMode);
		}
		return changed;
	}
}

void ExposureSettingsEditor::DrawSettings(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings)
{
	DrawDefaultExposureModes(settingsController, settings);
	DrawDefaultExposureValues(settingsController, settings);
}

bool ExposureSettingsEditor::DrawOverrides(ViewportExposureOverrides& exposure, const EngineRenderingSettingsState& defaults) noexcept
{
	if (!BeginOverrideTable())
	{
		return false;
	}
	bool changed = DrawOverrideModes(exposure, defaults);
	changed |= DrawOverrideValues(exposure, defaults);
	ImGui::EndTable();
	return changed;
}
