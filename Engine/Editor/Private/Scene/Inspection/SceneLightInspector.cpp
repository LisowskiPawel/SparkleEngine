#include "PCH.h"
#include "Scene/Inspection/SceneLightInspector.h"

#include "Core/Public/Math/MathUtils.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "Scene/Lighting/PointLightDesc.h"
#include "Scene/Lighting/RectLightDesc.h"
#include "Scene/Lighting/SceneDirectionalLightDesc.h"
#include "Scene/Lighting/SceneLightDesc.h"
#include "Scene/Lighting/SpotLightDesc.h"
#include "Widgets/UiDetailsWidgets.h"

#include <algorithm>
#include <utility>

namespace SceneLightInspector
{
	static constexpr float kAngularSizeDragSpeedDegrees = 0.1f;
	static constexpr float kAngularSizeSliderMaxDegrees = 30.0f;
	static constexpr float kRadiusSliderMax = 25.0f;
	static constexpr float kAreaLightSizeSliderMax = 100.0f;
	static constexpr float kDirectionSliderMin = -1.0f;
	static constexpr float kDirectionSliderMax = 1.0f;
	static constexpr float kRangeSliderMax = 500.0f;

	static bool BuildLightCommonCategory(const std::string& filterText, SceneLightDesc& lightDesc) noexcept;
	static bool BuildDirectionalLightTransformCategory(const std::string& filterText, SceneDirectionalLightDesc& lightDesc) noexcept;
	static bool BuildDirectionalLightCategory(const std::string& filterText, SceneDirectionalLightDesc& lightDesc) noexcept;
	static bool BuildPointLightCategory(const std::string& filterText, PointLightDesc& lightDesc) noexcept;
	static bool BuildSpotLightCategory(const std::string& filterText, SpotLightDesc& lightDesc) noexcept;
	static bool BuildRectLightCategory(const std::string& filterText, RectLightDesc& lightDesc) noexcept;

	static bool EditLuminousIntensity(float& value) noexcept
	{
		float luminousIntensity = value;
		constexpr float kDefaultLuminousIntensity = 1.0f;
		const bool luminousIntensityEdited = EditorWidgets::EditDetailsFloat("Luminous Intensity (candela)", luminousIntensity, 10.0f, 0.0f, 0.0f, "%.3f", &kDefaultLuminousIntensity);
		
		if (luminousIntensityEdited)
		{
			value = luminousIntensity;
			return true;
		}
		return false;
	}

	static bool EditLightDirection(DirectX::XMFLOAT3& value) noexcept
	{
		float directionValues[3] = {value.x, value.y, value.z};
		const float defaultDirection[3] = {0.0f, -1.0f, 0.0f};
		const bool lightDirectionEdited = EditorWidgets::EditDetailsFloat3("Direction", directionValues, 0.01f, kDirectionSliderMin, kDirectionSliderMax, "%.3f", defaultDirection);
		
		if (lightDirectionEdited)
		{
			value = {directionValues[0], directionValues[1], directionValues[2]};
			return true;
		}
		return false;
	}

	static bool EditLightRange(float& value) noexcept
	{
		float range = value;
		constexpr float kDefaultRange = 0.0f;
		if (EditorWidgets::EditDetailsFloat("Range", range, 0.05f, 0.0f, kRangeSliderMax, "%.3f", &kDefaultRange))
		{
			value = range;
			return true;
		}
		return false;
	}

	static bool EditLightRadius(float& value) noexcept
	{
		float radius = value;
		constexpr float kDefaultRadius = 0.05f;
		if (EditorWidgets::EditDetailsFloat("Radius", radius, 0.01f, 0.0f, kRadiusSliderMax, "%.3f", &kDefaultRadius))
		{
			value = radius;
			return true;
		}
		return false;
	}

	static bool EditLightCastShadow(bool& value) noexcept
	{
		bool castShadow = value;
		constexpr bool kDefaultCastShadow = true;
		if (EditorWidgets::EditDetailsCheckbox("Cast Shadow", castShadow, &kDefaultCastShadow))
		{
			value = castShadow;
			return true;
		}
		return false;
	}

	static bool BuildLightTypeCategories(const std::string& filterText, SceneLightDesc& lightDesc) noexcept
	{
		if (SceneDirectionalLightDesc* directional = lightDesc.GetDirectional())
		{
			bool changed = BuildDirectionalLightTransformCategory(filterText, *directional);
			changed |= BuildDirectionalLightCategory(filterText, *directional);
			return changed;
		}
		else if (PointLightDesc* point = lightDesc.GetPoint())
		{
			return BuildPointLightCategory(filterText, *point);
		}
		else if (SpotLightDesc* spot = lightDesc.GetSpot())
		{
			return BuildSpotLightCategory(filterText, *spot);
		}
		else if (RectLightDesc* rect = lightDesc.GetRect())
		{
			return BuildRectLightCategory(filterText, *rect);
		}

		return false;
	}

	void Build(const SceneLightDesc& sceneLight, EntityId lightEntity, EditorTransactionHistory& transactionHistory, std::uint64_t worldGeneration, const std::string& filterText) noexcept
	{
		SceneLightDesc lightDesc = sceneLight;
		bool changed = BuildLightCommonCategory(filterText, lightDesc);

		changed |= BuildLightTypeCategories(filterText, lightDesc);

		if (!changed)
		{
			return;
		}
		(void) transactionHistory
		    .Execute({0, SetLightDescriptionCommand{lightEntity, std::move(lightDesc)}}, {0, SetLightDescriptionCommand{lightEntity, sceneLight}}, worldGeneration, "light-description");
	}

	static bool BuildLightCommonCategory(const std::string& filterText, SceneLightDesc& lightDesc) noexcept
	{
		bool changed = false;
		const bool lightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Light", "color visible visibility rendering");
		const bool lightOpen = lightMatches && EditorWidgets::DrawDetailsCategory("Light");

		if (!lightOpen)
		{
			return false;
		}

		float colorValues[3] = {lightDesc.common.color.x, lightDesc.common.color.y, lightDesc.common.color.z};
		const float defaultColor[3] = {1.0f, 1.0f, 1.0f};
		if (EditorWidgets::EditDetailsColor3("Color", colorValues, defaultColor))
		{
			lightDesc.common.color = {colorValues[0], colorValues[1], colorValues[2]};
			changed = true;
		}

		constexpr bool kDefaultVisible = true;
		bool visible = lightDesc.common.visible;
		if (EditorWidgets::EditDetailsCheckbox("Visible", visible, &kDefaultVisible))
		{
			lightDesc.common.visible = visible;
			changed = true;
		}
		return changed;
	}

	static bool BuildDirectionalLightTransformCategory(const std::string& filterText, SceneDirectionalLightDesc& lightDesc) noexcept
	{
		const bool transformMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Transform", "rotation direction transform");
		const bool transformOpen = transformMatches && EditorWidgets::DrawDetailsCategory("Transform");

		if (!transformOpen)
		{
			return false;
		}

		DirectX::XMFLOAT3 rotationDegrees = MathUtils::DirectionToRotationDegrees(lightDesc.direction);
		float rotationValues[3] = {rotationDegrees.x, rotationDegrees.y, rotationDegrees.z};
		const DirectX::XMFLOAT3 defaultRotation = MathUtils::DirectionToRotationDegrees(DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f});
		const float defaultRotationValues[3] = {defaultRotation.x, defaultRotation.y, defaultRotation.z};
		if (EditorWidgets::EditDetailsFloat3("Rotation", rotationValues, 0.1f, -360.0f, 360.0f, "%.2f", defaultRotationValues))
		{
			lightDesc.direction = MathUtils::RotationDegreesToDirection(DirectX::XMFLOAT3{rotationValues[0], rotationValues[1], rotationValues[2]});
			return true;
		}
		return false;
	}

	static bool BuildDirectionalLightCategory(const std::string& filterText, SceneDirectionalLightDesc& lightDesc) noexcept
	{
		bool changed = false;
		const bool directionalLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Directional Light", "illuminance lux angular size direction cast shadow rendering");
		const bool directionalLightOpen = directionalLightMatches && EditorWidgets::DrawDetailsCategory("Directional Light");

		if (!directionalLightOpen)
		{
			return false;
		}

		float illuminance = lightDesc.illuminance;
		constexpr float kDefaultIlluminance = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Illuminance (lux)", illuminance, 100.0f, 0.0f, 0.0f, "%.3f", &kDefaultIlluminance))
		{
			lightDesc.illuminance = illuminance;
			changed = true;
		}

		changed |= EditLightDirection(lightDesc.direction);

		changed |= EditLightCastShadow(lightDesc.castShadow);

		float angularSizeDegrees = MathUtils::RadiansToDegrees(lightDesc.angularSizeRadians);
		constexpr float kDefaultAngularSizeDegrees = 0.533f;
		const bool angularSizeEdited = EditorWidgets::EditDetailsFloat("Angular Size", angularSizeDegrees, kAngularSizeDragSpeedDegrees, 0.0f, kAngularSizeSliderMaxDegrees, "%.3f", &kDefaultAngularSizeDegrees);
		
		if (angularSizeEdited)
		{
			lightDesc.angularSizeRadians = MathUtils::DegreesToRadians(angularSizeDegrees);
			changed = true;
		}
		return changed;
	}

	static bool BuildPointLightCategory(const std::string& filterText, PointLightDesc& lightDesc) noexcept
	{
		bool changed = false;
		const bool pointLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Point Light", "luminous intensity candela range radius distance attenuation point");
		const bool pointLightOpen = pointLightMatches && EditorWidgets::DrawDetailsCategory("Point Light");

		if (!pointLightOpen)
		{
			return false;
		}

		changed |= EditLuminousIntensity(lightDesc.luminousIntensity);

		changed |= EditLightRange(lightDesc.range);

		changed |= EditLightRadius(lightDesc.radius);
		return changed;
	}

	static bool BuildSpotLightCategory(const std::string& filterText, SpotLightDesc& lightDesc) noexcept
	{
		bool changed = false;
		const bool spotLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Spot Light", "luminous intensity candela direction range radius distance attenuation inner outer angle spot");
		const bool spotLightOpen = spotLightMatches && EditorWidgets::DrawDetailsCategory("Spot Light");

		if (!spotLightOpen)
		{
			return false;
		}

		changed |= EditLuminousIntensity(lightDesc.luminousIntensity);

		changed |= EditLightDirection(lightDesc.direction);

		changed |= EditLightRange(lightDesc.range);

		changed |= EditLightRadius(lightDesc.radius);

		float innerAngleDegrees = MathUtils::RadiansToDegrees(lightDesc.innerAngleRadians);
		constexpr float kDefaultInnerAngleDegrees = 0.0f;
		const bool innerAngleEdited = EditorWidgets::EditDetailsFloat("Inner Angle", innerAngleDegrees, 0.1f, 0.0f, 180.0f, "%.2f", &kDefaultInnerAngleDegrees);
		
		if (innerAngleEdited)
		{
			lightDesc.innerAngleRadians = MathUtils::DegreesToRadians(innerAngleDegrees);
			changed = true;
		}

		float outerAngleDegrees = MathUtils::RadiansToDegrees(lightDesc.outerAngleRadians);
		constexpr float kDefaultOuterAngleDegrees = 45.0f;
		const bool outerAngleEdited = EditorWidgets::EditDetailsFloat("Outer Angle", outerAngleDegrees, 0.1f, 0.0f, 180.0f, "%.2f", &kDefaultOuterAngleDegrees);
		
		if (outerAngleEdited)
		{
			lightDesc.outerAngleRadians = MathUtils::DegreesToRadians(outerAngleDegrees);
			changed = true;
		}
		return changed;
	}

	static bool BuildRectLightCategory(const std::string& filterText, RectLightDesc& lightDesc) noexcept
	{
		bool changed = false;
		const bool rectLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Rect Light", "luminance candela square meter direction tangent width height area rectangle quad shadow");
		const bool rectLightOpen = rectLightMatches && EditorWidgets::DrawDetailsCategory("Rect Light");

		if (!rectLightOpen)
		{
			return false;
		}

		float luminance = lightDesc.luminance;
		constexpr float kDefaultLuminance = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Luminance (cd/m^2)", luminance, 10.0f, 0.0f, 0.0f, "%.3f", &kDefaultLuminance))
		{
			lightDesc.luminance = luminance;
			changed = true;
		}

		changed |= EditLightDirection(lightDesc.direction);

		float tangentValues[3] = {lightDesc.tangent.x, lightDesc.tangent.y, lightDesc.tangent.z};
		const float defaultTangent[3] = {1.0f, 0.0f, 0.0f};
		const bool lightTangentEdited = EditorWidgets::EditDetailsFloat3("Tangent", tangentValues, 0.01f, kDirectionSliderMin, kDirectionSliderMax, "%.3f", defaultTangent);
		
		if (lightTangentEdited)
		{
			lightDesc.tangent = {tangentValues[0], tangentValues[1], tangentValues[2]};
			changed = true;
		}

		float width = lightDesc.width;
		constexpr float kDefaultSize = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Width", width, 0.05f, 0.0f, kAreaLightSizeSliderMax, "%.3f", &kDefaultSize))
		{
			lightDesc.width = width;
			changed = true;
		}

		float height = lightDesc.height;
		if (EditorWidgets::EditDetailsFloat("Height", height, 0.05f, 0.0f, kAreaLightSizeSliderMax, "%.3f", &kDefaultSize))
		{
			lightDesc.height = height;
			changed = true;
		}

		changed |= EditLightCastShadow(lightDesc.castShadow);
		return changed;
	}
}
