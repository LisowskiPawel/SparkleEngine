#include "PCH.h"
#include "Scene/Inspection/SceneSkyInspector.h"

#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Widgets/UiDetailsWidgets.h"
#include "World/SkyEnvironment.h"

#include <algorithm>

void SceneSkyInspector::Build(const std::optional<SkyEnvironment>& current, EditorTransactionHistory& transactionHistory, std::uint64_t generation, const std::string& filter) noexcept
{
	bool hasSky = current.has_value();
	const bool skyMatches = EditorWidgets::MatchesDetailsFilter(filter, "Sky", "override default texture color brightness enabled visible");
	const bool skyOpen = skyMatches && EditorWidgets::DrawDetailsCategory("Sky");

	if (skyOpen)
	{
		const bool defaultHasSky = false;
		if (EditorWidgets::EditDetailsCheckbox("Override Default", hasSky, &defaultHasSky))
		{
			std::optional<SkyEnvironment> after = hasSky ? std::optional<SkyEnvironment>{SkyEnvironment{}} : std::nullopt;
			(void) transactionHistory.Execute({0, SetSkyEnvironmentCommand{after}}, {0, SetSkyEnvironmentCommand{current}}, generation);
		}
		if (!hasSky)
		{
			EditorWidgets::DrawDetailsValueRow("Source", "Engine Default");
		}
	}
	if (!hasSky || !current)
	{
		return;
	}

	SceneSkyDesc after = *current;
	bool changed = false;
	const bool renderingMatches = EditorWidgets::MatchesDetailsFilter(filter, "Rendering", "sky texture color brightness enabled visible");
	const bool renderingOpen = renderingMatches && EditorWidgets::DrawDetailsCategory("Rendering");

	if (renderingOpen)
	{
		const bool defaultEnabled = true;
		changed |= EditorWidgets::EditDetailsCheckbox("Enabled", after.enabled, &defaultEnabled);
		float color[3] = {after.color.x, after.color.y, after.color.z};
		const float defaultColor[3] = {1.0f, 1.0f, 1.0f};
		if (EditorWidgets::EditDetailsColor3("Color", color, defaultColor))
		{
			after.color = {(std::max) (0.0f, color[0]), (std::max) (0.0f, color[1]), (std::max) (0.0f, color[2])};
			changed = true;
		}

		const float defaultBrightness = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Brightness", after.brightness, 0.01f, 0.0f, 0.0f, "%.3f", &defaultBrightness))
		{
			after.brightness = (std::max) (0.0f, after.brightness);
			changed = true;
		}

		const std::string defaultTexture;
		if (EditorWidgets::EditDetailsText("Texture", after.skyTexture.texturePath, &defaultTexture))
		{
			after.skyTexture.textureGroup = TextureGroup::HdrColor;
			changed = true;
		}
	}
	if (changed)
	{
		(void) transactionHistory.Execute({0, SetSkyEnvironmentCommand{SkyEnvironment{after}}}, {0, SetSkyEnvironmentCommand{current}}, generation, "sky-environment");
	}
}
