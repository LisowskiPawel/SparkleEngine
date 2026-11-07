#include "PCH.h"
#include "Scene/Inspection/SceneSkyInspector.h"

#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Widgets/UiDetailsWidgets.h"
#include "World/SkyEnvironment.h"

#include <algorithm>

void SceneSkyInspector::Build(const std::optional<SkyEnvironment>& current, EditorTransactionHistory& transactionHistory, std::uint64_t generation, const std::string& filter) noexcept
{
	bool bHasSky = current.has_value();
	const bool bSkyMatches = EditorWidgets::MatchesDetailsFilter(filter, "Sky", "override default texture color brightness enabled visible");
	const bool bSkyOpen = bSkyMatches && EditorWidgets::DrawDetailsCategory("Sky");

	if (bSkyOpen)
	{
		const bool bDefaultHasSky = false;
		if (EditorWidgets::EditDetailsCheckbox("Override Default", bHasSky, &bDefaultHasSky))
		{
			std::optional<SkyEnvironment> after = bHasSky ? std::optional<SkyEnvironment>{SkyEnvironment{}} : std::nullopt;
			(void) transactionHistory.Execute({0, SetSkyEnvironmentCommand{after}}, {0, SetSkyEnvironmentCommand{current}}, generation);
		}
		if (!bHasSky)
		{
			EditorWidgets::DrawDetailsValueRow("Source", "Engine Default");
		}
	}
	if (!bHasSky || !current)
	{
		return;
	}

	SceneSkyDesc after = *current;
	bool bChanged = false;
	const bool bRenderingMatches = EditorWidgets::MatchesDetailsFilter(filter, "Rendering", "sky texture color brightness enabled visible");
	const bool bRenderingOpen = bRenderingMatches && EditorWidgets::DrawDetailsCategory("Rendering");

	if (bRenderingOpen)
	{
		const bool bDefaultEnabled = true;
		bChanged |= EditorWidgets::EditDetailsCheckbox("Enabled", after.bEnabled, &bDefaultEnabled);
		float color[3] = {after.color.x, after.color.y, after.color.z};
		const float defaultColor[3] = {1.0f, 1.0f, 1.0f};
		if (EditorWidgets::EditDetailsColor3("Color", color, defaultColor))
		{
			after.color = {(std::max) (0.0f, color[0]), (std::max) (0.0f, color[1]), (std::max) (0.0f, color[2])};
			bChanged = true;
		}

		const float defaultBrightness = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Brightness", after.brightness, 0.01f, 0.0f, 0.0f, "%.3f", &defaultBrightness))
		{
			after.brightness = (std::max) (0.0f, after.brightness);
			bChanged = true;
		}

		const std::string defaultTexture;
		if (EditorWidgets::EditDetailsText("Texture", after.skyTexture.texturePath, &defaultTexture))
		{
			after.skyTexture.textureGroup = TextureGroup::HdrColor;
			bChanged = true;
		}
	}
	if (bChanged)
	{
		(void) transactionHistory.Execute({0, SetSkyEnvironmentCommand{SkyEnvironment{after}}}, {0, SetSkyEnvironmentCommand{current}}, generation, "sky-environment");
	}
}
