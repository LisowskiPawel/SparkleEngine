#include "PCH.h"

#include "LightFieldParser.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Strings/StringUtils.h"

#include <format>
#include <type_traits>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_lightFieldParserLogger, "GameFramework.LightFieldParser");

namespace LevelParsing
{
	[[noreturn]] static void UnsupportedField(SceneLightKind kind, std::string_view field);

	static void SetLightPosition(SceneLightCommonDesc& common, const DirectX::XMFLOAT3& position) noexcept
	{
		common.worldTransform._41 = position.x;
		common.worldTransform._42 = position.y;
		common.worldTransform._43 = position.z;
	}

	static std::string_view GetLightKindName(SceneLightKind kind) noexcept
	{
		switch (kind)
		{
			case SceneLightKind::Directional:
				return "directional";
			case SceneLightKind::Point:
				return "point";
			case SceneLightKind::Spot:
				return "spot";
			case SceneLightKind::Rect:
				return "rect";
			case SceneLightKind::Unknown:
			default:
				return "unknown";
		}
	}

	static SceneLightDesc CreateLight(SceneLightKind kind)
	{
		SceneLightDesc light;
		switch (kind)
		{
			case SceneLightKind::Directional:
				light.payload = SceneDirectionalLightDesc{};

				return light;

			case SceneLightKind::Point:
				light.payload = PointLightDesc{};

				return light;

			case SceneLightKind::Spot:
				light.payload = SpotLightDesc{};

				return light;

			case SceneLightKind::Rect:
				light.payload = RectLightDesc{};

				return light;
			case SceneLightKind::Unknown:
			default:
				Diagnostics::Fatal(g_lightFieldParserLogger, __FILE__, __LINE__, "Light field parser received an unknown light kind.");
		}
	}

	static SceneLightDesc& ResolveLight(std::vector<SceneLightDesc>& lights, SceneLightKind kind, std::size_t kindIndex)
	{
		std::size_t currentKindIndex = 0;
		for (SceneLightDesc& light : lights)
		{
			if (light.GetKind() != kind)
			{
				continue;
			}
			if (currentKindIndex == kindIndex)
			{
				return light;
			}
			++currentKindIndex;
		}

		if (currentKindIndex != kindIndex)
		{
			throw Diagnostics::Error("Light indices must be contiguous for each light kind.");
		}
		lights.push_back(CreateLight(kind));
		return lights.back();
	}

	static bool IsCommonField(std::string_view field) noexcept
	{
		return field == "Name" || field == "Color" || field == "Visible" || field == "Position";
	}

	static void ParseCommonField(std::string_view field, const ParsedLevelLine& line, SceneLightCommonDesc& common)
	{
		if (field == "Name")
		{
			common.name = Strings::UnquoteCopy(line.value);
			if (common.name.empty())
			{
				throw Diagnostics::Error("Light name cannot be empty.");
			}
			return;
		}
		if (field == "Color")
		{
			common.color = ParseFloat3(line.value, "light color");
			return;
		}
		if (field == "Visible")
		{
			common.bVisible = ParseBool(line.value, "light visibility");
			return;
		}
		if (field == "Position")
		{
			const DirectX::XMFLOAT3 position = ParseFloat3(line.value, "light position");
			SetLightPosition(common, position);
			return;
		}
	}

	static void ParseDirectionalField(std::string_view field, const ParsedLevelLine& line, SceneLightDesc& light)
	{
		SceneDirectionalLightDesc& value = *light.GetDirectional();
		if (field == "Direction")
		{
			value.direction = ParseFloat3(line.value, "directional light direction");
		}
		else if (field == "CastShadow")
		{
			value.bCastShadow = ParseBool(line.value, "directional light shadow flag");
		}
		else if (field == "IlluminanceLux")
		{
			value.illuminance = ParseFloat(line.value, "directional light illuminance");
		}
		else if (field == "AngularSizeRadians")
		{
			value.angularSizeRadians = ParseFloat(line.value, "directional light angular size");
		}
		else
		{
			UnsupportedField(SceneLightKind::Directional, field);
		}
	}

	template <typename TLight> static bool ParseLocalLightField(std::string_view field, const ParsedLevelLine& line, TLight& value)
	{
		constexpr bool bSpot = std::is_same_v<TLight, SpotLightDesc>;
		if (field == "LuminousIntensityCandela")
		{
			value.luminousIntensity = ParseFloat(line.value, bSpot ? "spot light luminous intensity" : "point light luminous intensity");
		}
		else if (field == "Range")
		{
			value.range = ParseFloat(line.value, bSpot ? "spot light range" : "point light range");
		}
		else if (field == "Radius")
		{
			value.radius = ParseFloat(line.value, bSpot ? "spot light radius" : "point light radius");
		}
		else if (field == "DistanceAttenuationCoefficients")
		{
			value.distanceAttenuationCoefficients = ParseFloat3(line.value, bSpot ? "spot light distance attenuation coefficients" : "point light distance attenuation coefficients");
		}
		else if (field == "CastShadow")
		{
			value.bCastShadow = ParseBool(line.value, bSpot ? "spot light shadow flag" : "point light shadow flag");
		}
		else
		{
			return false;
		}
		return true;
	}

	static void ParseSpotField(std::string_view field, const ParsedLevelLine& line, SceneLightDesc& light)
	{
		SpotLightDesc& value = *light.GetSpot();
		if (ParseLocalLightField(field, line, value))
		{
			return;
		}
		if (field == "Direction")
		{
			value.direction = ParseFloat3(line.value, "spot light direction");
		}
		else if (field == "InnerAngleRadians")
		{
			value.innerAngleRadians = ParseFloat(line.value, "spot light inner angle");
		}
		else if (field == "OuterAngleRadians")
		{
			value.outerAngleRadians = ParseFloat(line.value, "spot light outer angle");
		}
		else
		{
			UnsupportedField(SceneLightKind::Spot, field);
		}
	}

	static void ParseRectField(std::string_view field, const ParsedLevelLine& line, SceneLightDesc& light)
	{
		RectLightDesc& value = *light.GetRect();
		if (field == "LuminanceCdPerM2")
		{
			value.luminance = ParseFloat(line.value, "rect light luminance");
		}
		else if (field == "Direction")
		{
			value.direction = ParseFloat3(line.value, "rect light direction");
		}
		else if (field == "Tangent")
		{
			value.tangent = ParseFloat3(line.value, "rect light tangent");
		}
		else if (field == "Width")
		{
			value.width = ParseFloat(line.value, "rect light width");
		}
		else if (field == "Height")
		{
			value.height = ParseFloat(line.value, "rect light height");
		}
		else if (field == "CastShadow")
		{
			value.bCastShadow = ParseBool(line.value, "rect light shadow flag");
		}
		else
		{
			UnsupportedField(SceneLightKind::Rect, field);
		}
	}

	[[noreturn]] static void UnsupportedField(SceneLightKind kind, std::string_view field)
	{
		throw Diagnostics::Error(std::format("Unsupported {} light field '{}'.", GetLightKindName(kind), field));
	}

	void ParseLightField(const ParsedLightFieldKey& key, const ParsedLevelLine& parsedLine, std::vector<SceneLightDesc>& lights)
	{
		SceneLightDesc& light = ResolveLight(lights, key.Kind, key.Index);
		if (IsCommonField(key.Field))
		{
			ParseCommonField(key.Field, parsedLine, light.common);
			return;
		}
		switch (key.Kind)
		{
			case SceneLightKind::Directional:
				ParseDirectionalField(key.Field, parsedLine, light);
				return;
			case SceneLightKind::Point:
				if (!ParseLocalLightField(key.Field, parsedLine, *light.GetPoint()))
				{
					UnsupportedField(SceneLightKind::Point, key.Field);
				}
				return;
			case SceneLightKind::Spot:
				ParseSpotField(key.Field, parsedLine, light);
				return;
			case SceneLightKind::Rect:
				ParseRectField(key.Field, parsedLine, light);
				return;
			case SceneLightKind::Unknown:
			default:
				Diagnostics::Fatal(g_lightFieldParserLogger, __FILE__, __LINE__, "Light field parser received an unknown light kind.");
		}
	}

}
