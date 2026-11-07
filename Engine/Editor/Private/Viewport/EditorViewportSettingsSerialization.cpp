#include "PCH.h"

#include "Viewport/EditorViewportSettingsSerialization.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Viewport/EditorViewportSettings.h"

#include <charconv>
#include <cmath>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

namespace EditorViewportSettingsSerialization
{
	template <typename TState, typename TVisitor> static void VisitSettings(TState& state, TVisitor&& visit)
	{
		visit("MoveSpeedMetersPerSecond", state.Navigation.MoveSpeedMetersPerSecond);
		visit("RotationSpeedDegreesPerPixel", state.Navigation.RotationSpeedDegreesPerPixel);
		visit("InvertY", state.Navigation.InvertY);
		visit("ProjectionKind", state.ProjectionKind);
		visit("OrthographicHeightMeters", state.OrthographicHeightMeters);
		visit("OverrideExposureMode", state.Exposure.OverrideMode);
		visit("ExposureMode", state.Exposure.Mode);
		visit("OverrideExposureMeteringMethod", state.Exposure.OverrideMeteringMethod);
		visit("ExposureMeteringMethod", state.Exposure.MeteringMethod);
		visit("OverrideManualExposure", state.Exposure.OverrideManualExposure);
		visit("ManualExposure", state.Exposure.ManualExposure);
		visit("OverrideExposureCompensation", state.Exposure.OverrideCompensation);
		visit("ExposureCompensation", state.Exposure.Compensation);
		visit("OverrideExposureTargetLuminance", state.Exposure.OverrideTargetLuminance);
		visit("ExposureTargetLuminance", state.Exposure.TargetLuminance);
		visit("OverrideExposureMinimum", state.Exposure.OverrideMinimum);
		visit("ExposureMinimum", state.Exposure.Minimum);
		visit("OverrideExposureMaximum", state.Exposure.OverrideMaximum);
		visit("ExposureMaximum", state.Exposure.Maximum);
		visit("OverrideExposureAdaptationSpeedUp", state.Exposure.OverrideAdaptationSpeedUp);
		visit("ExposureAdaptationSpeedUp", state.Exposure.AdaptationSpeedUp);
		visit("OverrideExposureAdaptationSpeedDown", state.Exposure.OverrideAdaptationSpeedDown);
		visit("ExposureAdaptationSpeedDown", state.Exposure.AdaptationSpeedDown);
	}

	template <typename TValue> static void ParseValue(std::string_view text, TValue& value) noexcept
	{
		using Number = std::conditional_t<std::is_enum_v<TValue> || std::is_same_v<TValue, bool>, int, TValue>;
		const std::string_view trimmed = Strings::TrimAsciiWhitespace(text);
		if (trimmed.empty())
		{
			return;
		}
		const char* begin = trimmed.data();
		const char* end = begin + trimmed.size();
		Number parsed{};
		const auto result = std::from_chars(begin, end, parsed);
		if (result.ec != std::errc{} || result.ptr != end || !std::isfinite(parsed))
		{
			return;
		}
		if constexpr (std::is_same_v<TValue, bool>)
		{
			if (parsed != 0 && parsed != 1)
			{
				return;
			}
		}
		else if constexpr (std::is_enum_v<TValue>)
		{
			if (!std::in_range<std::underlying_type_t<TValue>>(parsed))
			{
				return;
			}
		}
		value = static_cast<TValue>(parsed);
	}

	static void ApplySetting(std::string_view key, std::string_view value, EditorViewportSettingsState& state) noexcept
	{
		VisitSettings(
		    state,
		    [key, value](std::string_view fieldKey, auto& field)
		    {
			    if (key == fieldKey)
			    {
				    ParseValue(value, field);
			    }
		    });
	}
}

void EditorViewportSettingsSerialization::Read(std::istream& input, EditorViewportSettingsState& state)
{
	bool bInSettingsSection = false;
	for (std::string line; std::getline(input, line);)
	{
		const std::string trimmed = Strings::TrimCopy(line);
		if (trimmed.empty() || trimmed.starts_with(';') || trimmed.starts_with('#'))
		{
			continue;
		}
		if (trimmed.starts_with('[') && trimmed.ends_with(']'))
		{
			bInSettingsSection = trimmed == "[EditorViewport]";
			continue;
		}
		if (!bInSettingsSection)
		{
			continue;
		}
		const std::size_t separator = trimmed.find('=');
		if (separator != std::string::npos)
		{
			ApplySetting(std::string_view(trimmed).substr(0, separator), std::string_view(trimmed).substr(separator + 1), state);
		}
	}
}

void EditorViewportSettingsSerialization::Write(std::ostream& output, const EditorViewportSettingsState& state)
{
	output << "[EditorViewport]\n";

	VisitSettings(
	    state,
	    [&output](std::string_view key, const auto& value)
	    {
		    output << key << '=';
		    using Value = std::remove_cvref_t<decltype(value)>;
		    if constexpr (std::is_same_v<Value, bool> || std::is_enum_v<Value>)
		    {
			    output << static_cast<int>(value);
		    }
		    else
		    {
			    output << value;
		    }
		    output << '\n';
	    });
}
