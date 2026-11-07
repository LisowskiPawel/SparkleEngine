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
		const bool bLuminousIntensityEdited = EditorWidgets::EditDetailsFloat("Luminous Intensity (candela)", luminousIntensity, 10.0f, 0.0f, 0.0f, "%.3f", &kDefaultLuminousIntensity);

		if (bLuminousIntensityEdited)
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
		const bool bLightDirectionEdited = EditorWidgets::EditDetailsFloat3("Direction", directionValues, 0.01f, kDirectionSliderMin, kDirectionSliderMax, "%.3f", defaultDirection);

		if (bLightDirectionEdited)
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

	static bool EditLightCastShadow(bool& bValue) noexcept
	{
		bool bCastShadow = bValue;
		constexpr bool bKDefaultCastShadow = true;
		if (EditorWidgets::EditDetailsCheckbox("Cast Shadow", bCastShadow, &bKDefaultCastShadow))
		{
			bValue = bCastShadow;
			return true;
		}
		return false;
	}

	static bool BuildLightTypeCategories(const std::string& filterText, SceneLightDesc& lightDesc) noexcept
	{
		if (SceneDirectionalLightDesc* directional = lightDesc.GetDirectional())
		{
			bool bChanged = BuildDirectionalLightTransformCategory(filterText, *directional);
			bChanged |= BuildDirectionalLightCategory(filterText, *directional);
			return bChanged;
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
		bool bChanged = BuildLightCommonCategory(filterText, lightDesc);

		bChanged |= BuildLightTypeCategories(filterText, lightDesc);

		if (!bChanged)
		{
			return;
		}
		(void) transactionHistory
		    .Execute({0, SetLightDescriptionCommand{lightEntity, std::move(lightDesc)}}, {0, SetLightDescriptionCommand{lightEntity, sceneLight}}, worldGeneration, "light-description");
	}

	static bool BuildLightCommonCategory(const std::string& filterText, SceneLightDesc& lightDesc) noexcept
	{
		bool bChanged = false;
		const bool bLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Light", "color visible visibility rendering");
		const bool bLightOpen = bLightMatches && EditorWidgets::DrawDetailsCategory("Light");

		if (!bLightOpen)
		{
			return false;
		}

		float colorValues[3] = {lightDesc.common.color.x, lightDesc.common.color.y, lightDesc.common.color.z};
		const float defaultColor[3] = {1.0f, 1.0f, 1.0f};
		if (EditorWidgets::EditDetailsColor3("Color", colorValues, defaultColor))
		{
			lightDesc.common.color = {colorValues[0], colorValues[1], colorValues[2]};
			bChanged = true;
		}

		constexpr bool bKDefaultVisible = true;
		bool bVisible = lightDesc.common.bVisible;
		if (EditorWidgets::EditDetailsCheckbox("Visible", bVisible, &bKDefaultVisible))
		{
			lightDesc.common.bVisible = bVisible;
			bChanged = true;
		}
		return bChanged;
	}

	static bool BuildDirectionalLightTransformCategory(const std::string& filterText, SceneDirectionalLightDesc& lightDesc) noexcept
	{
		const bool bTransformMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Transform", "rotation direction transform");
		const bool bTransformOpen = bTransformMatches && EditorWidgets::DrawDetailsCategory("Transform");

		if (!bTransformOpen)
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
		bool bChanged = false;
		const bool bDirectionalLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Directional Light", "illuminance lux angular size direction cast shadow rendering");
		const bool bDirectionalLightOpen = bDirectionalLightMatches && EditorWidgets::DrawDetailsCategory("Directional Light");

		if (!bDirectionalLightOpen)
		{
			return false;
		}

		float illuminance = lightDesc.illuminance;
		constexpr float kDefaultIlluminance = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Illuminance (lux)", illuminance, 100.0f, 0.0f, 0.0f, "%.3f", &kDefaultIlluminance))
		{
			lightDesc.illuminance = illuminance;
			bChanged = true;
		}

		bChanged |= EditLightDirection(lightDesc.direction);

		bChanged |= EditLightCastShadow(lightDesc.bCastShadow);

		float angularSizeDegrees = MathUtils::RadiansToDegrees(lightDesc.angularSizeRadians);
		constexpr float kDefaultAngularSizeDegrees = 0.533f;

		const bool bAngularSizeEdited = EditorWidgets::EditDetailsFloat(
		    "Angular Size",
		    angularSizeDegrees,
		    kAngularSizeDragSpeedDegrees,
		    0.0f,
		    kAngularSizeSliderMaxDegrees,
		    "%.3f",
		    &kDefaultAngularSizeDegrees);

		if (bAngularSizeEdited)
		{
			lightDesc.angularSizeRadians = MathUtils::DegreesToRadians(angularSizeDegrees);
			bChanged = true;
		}
		return bChanged;
	}

	static bool BuildPointLightCategory(const std::string& filterText, PointLightDesc& lightDesc) noexcept
	{
		bool bChanged = false;
		const bool bPointLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Point Light", "luminous intensity candela range radius distance attenuation point");
		const bool bPointLightOpen = bPointLightMatches && EditorWidgets::DrawDetailsCategory("Point Light");

		if (!bPointLightOpen)
		{
			return false;
		}

		bChanged |= EditLuminousIntensity(lightDesc.luminousIntensity);

		bChanged |= EditLightRange(lightDesc.range);

		bChanged |= EditLightRadius(lightDesc.radius);
		return bChanged;
	}

	static bool BuildSpotLightCategory(const std::string& filterText, SpotLightDesc& lightDesc) noexcept
	{
		bool bChanged = false;
		const bool bSpotLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Spot Light", "luminous intensity candela direction range radius distance attenuation inner outer angle spot");
		const bool bSpotLightOpen = bSpotLightMatches && EditorWidgets::DrawDetailsCategory("Spot Light");

		if (!bSpotLightOpen)
		{
			return false;
		}

		bChanged |= EditLuminousIntensity(lightDesc.luminousIntensity);

		bChanged |= EditLightDirection(lightDesc.direction);

		bChanged |= EditLightRange(lightDesc.range);

		bChanged |= EditLightRadius(lightDesc.radius);

		float innerAngleDegrees = MathUtils::RadiansToDegrees(lightDesc.innerAngleRadians);
		constexpr float kDefaultInnerAngleDegrees = 0.0f;
		const bool bInnerAngleEdited = EditorWidgets::EditDetailsFloat("Inner Angle", innerAngleDegrees, 0.1f, 0.0f, 180.0f, "%.2f", &kDefaultInnerAngleDegrees);

		if (bInnerAngleEdited)
		{
			lightDesc.innerAngleRadians = MathUtils::DegreesToRadians(innerAngleDegrees);
			bChanged = true;
		}

		float outerAngleDegrees = MathUtils::RadiansToDegrees(lightDesc.outerAngleRadians);
		constexpr float kDefaultOuterAngleDegrees = 45.0f;
		const bool bOuterAngleEdited = EditorWidgets::EditDetailsFloat("Outer Angle", outerAngleDegrees, 0.1f, 0.0f, 180.0f, "%.2f", &kDefaultOuterAngleDegrees);

		if (bOuterAngleEdited)
		{
			lightDesc.outerAngleRadians = MathUtils::DegreesToRadians(outerAngleDegrees);
			bChanged = true;
		}
		return bChanged;
	}

	static bool BuildRectLightCategory(const std::string& filterText, RectLightDesc& lightDesc) noexcept
	{
		bool bChanged = false;
		const bool bRectLightMatches = EditorWidgets::MatchesDetailsFilter(filterText, "Rect Light", "luminance candela square meter direction tangent width height area rectangle quad shadow");
		const bool bRectLightOpen = bRectLightMatches && EditorWidgets::DrawDetailsCategory("Rect Light");

		if (!bRectLightOpen)
		{
			return false;
		}

		float luminance = lightDesc.luminance;
		constexpr float kDefaultLuminance = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Luminance (cd/m^2)", luminance, 10.0f, 0.0f, 0.0f, "%.3f", &kDefaultLuminance))
		{
			lightDesc.luminance = luminance;
			bChanged = true;
		}

		bChanged |= EditLightDirection(lightDesc.direction);

		float tangentValues[3] = {lightDesc.tangent.x, lightDesc.tangent.y, lightDesc.tangent.z};
		const float defaultTangent[3] = {1.0f, 0.0f, 0.0f};
		const bool bLightTangentEdited = EditorWidgets::EditDetailsFloat3("Tangent", tangentValues, 0.01f, kDirectionSliderMin, kDirectionSliderMax, "%.3f", defaultTangent);

		if (bLightTangentEdited)
		{
			lightDesc.tangent = {tangentValues[0], tangentValues[1], tangentValues[2]};
			bChanged = true;
		}

		float width = lightDesc.width;
		constexpr float kDefaultSize = 1.0f;
		if (EditorWidgets::EditDetailsFloat("Width", width, 0.05f, 0.0f, kAreaLightSizeSliderMax, "%.3f", &kDefaultSize))
		{
			lightDesc.width = width;
			bChanged = true;
		}

		float height = lightDesc.height;
		if (EditorWidgets::EditDetailsFloat("Height", height, 0.05f, 0.0f, kAreaLightSizeSliderMax, "%.3f", &kDefaultSize))
		{
			lightDesc.height = height;
			bChanged = true;
		}

		bChanged |= EditLightCastShadow(lightDesc.bCastShadow);
		return bChanged;
	}
}
