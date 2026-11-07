#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"
#include "World/WorldTransformConversion.h"

class LightComponentTranslation final
{
public:
	static ECS::Light ToLightComponent(const SceneLightDesc& desc) noexcept
	{
		ECS::Light light;
		light.Kind = desc.GetKind();
		light.Color = desc.common.color;
		if (const SceneDirectionalLightDesc* directional = desc.GetDirectional())
		{
			light.Direction = directional->direction;
			light.Illuminance = directional->illuminance;
			light.AngularSizeRadians = directional->angularSizeRadians;
			light.bCastShadow = directional->bCastShadow;
		}
		else if (const PointLightDesc* point = desc.GetPoint())
		{
			light.LuminousIntensity = point->luminousIntensity;
			light.Range = point->range;
			light.Radius = point->radius;
			light.DistanceAttenuationCoefficients = point->distanceAttenuationCoefficients;
			light.bCastShadow = point->bCastShadow;
		}
		else if (const SpotLightDesc* spot = desc.GetSpot())
		{
			light.Direction = spot->direction;
			light.LuminousIntensity = spot->luminousIntensity;
			light.Range = spot->range;
			light.Radius = spot->radius;
			light.DistanceAttenuationCoefficients = spot->distanceAttenuationCoefficients;
			light.InnerAngleRadians = spot->innerAngleRadians;
			light.OuterAngleRadians = spot->outerAngleRadians;
			light.bCastShadow = spot->bCastShadow;
		}
		else if (const RectLightDesc* rect = desc.GetRect())
		{
			light.Direction = rect->direction;
			light.Luminance = rect->luminance;
			light.Tangent = rect->tangent;
			light.AreaSize = {rect->width, rect->height};
			light.bCastShadow = rect->bCastShadow;
		}
		return light;
	}

	static SceneLightPayload ToLightPayload(const ECS::Light& light)
	{
		switch (light.Kind)
		{
			case SceneLightKind::Directional:
				return SceneDirectionalLightDesc{.direction = light.Direction, .illuminance = light.Illuminance, .angularSizeRadians = light.AngularSizeRadians, .bCastShadow = light.bCastShadow};

			case SceneLightKind::Point:
				return PointLightDesc{
				    .luminousIntensity = light.LuminousIntensity,
				    .range = light.Range,
				    .radius = light.Radius,
				    .distanceAttenuationCoefficients = light.DistanceAttenuationCoefficients,
				    .bCastShadow = light.bCastShadow};

			case SceneLightKind::Spot:
				return SpotLightDesc{
				    .direction = light.Direction,
				    .luminousIntensity = light.LuminousIntensity,
				    .range = light.Range,
				    .radius = light.Radius,
				    .distanceAttenuationCoefficients = light.DistanceAttenuationCoefficients,
				    .innerAngleRadians = light.InnerAngleRadians,
				    .outerAngleRadians = light.OuterAngleRadians,
				    .bCastShadow = light.bCastShadow};

			case SceneLightKind::Rect:
				return RectLightDesc{
				    .direction = light.Direction,
				    .luminance = light.Luminance,
				    .width = light.AreaSize.x,
				    .tangent = light.Tangent,
				    .height = light.AreaSize.y,
				    .bCastShadow = light.bCastShadow};

			case SceneLightKind::Unknown:
			default:
				return std::monostate{};
		}
	}
};

namespace ECS
{
	EntityId GameWorldState::AddLight(SceneLightDesc&& desc)
	{
		const EntityId entity = m_registry.Create();
		if (!entity.IsValid())
		{
			return entity;
		}
		const Transform transform(DirectX::XMLoadFloat4x4(&desc.common.worldTransform));
		const LocalTransform local = WorldTransformConversion::ToLocal(transform);

		const bool bAddedAdd = m_registry.Add(entity, local) && m_registry.Add(entity, WorldTransform{});
		const bool bAddedAdd2 = bAddedAdd && (m_registry.Add(entity, LightComponentTranslation::ToLightComponent(desc)));
		const bool bAddedcommon = bAddedAdd2 && (m_registry.Add(entity, Visibility{.bVisible = desc.common.bVisible}));
		const bool bAddedcommon2 = bAddedcommon && (m_registry.Add(entity, Name{std::move(desc.common.name)}));
		const bool bAddedAdd3 = bAddedcommon2 && (m_registry.Add(entity, AuthoredIdentity{.SourceObjectId = ++m_nextLightIdentity, .Kind = AuthoredObjectKind::Light}));
		const bool bAdded = bAddedAdd3 && (m_registry.Add(entity, EditorMetadata{}));

		if (!bAdded)
		{
			m_registry.Destroy(entity);
			return EntityId::Invalid();
		}
		MarkTransformDirty(entity);
		RecordChange(entity);
		return entity;
	}

	std::optional<SceneLightDesc> GameWorldState::ReadLight(EntityId entity) const
	{
		const Light* light = m_registry.Get<Light>(entity);
		if (light == nullptr)
		{
			return std::nullopt;
		}
		SceneLightDesc desc;
		if (const Name* name = m_registry.Get<Name>(entity))
		{
			desc.common.name = name->Value;
		}
		if (const WorldTransform* transform = m_registry.Get<WorldTransform>(entity))
		{
			desc.common.worldTransform = transform->Matrix;
		}
		desc.common.color = light->Color;
		desc.common.bVisible = ReadVisibility(entity);
		desc.payload = LightComponentTranslation::ToLightPayload(*light);
		return desc;
	}

	bool GameWorldState::WriteLight(EntityId entity, SceneLightDesc&& desc)
	{
		if (m_registry.Get<Light>(entity) == nullptr)
		{
			return false;
		}
		const Transform transform(DirectX::XMLoadFloat4x4(&desc.common.worldTransform));

		const bool bWrittenReplace = WriteTransform(entity, transform) && m_registry.Replace(entity, LightComponentTranslation::ToLightComponent(desc));
		const bool bWrittencommon = bWrittenReplace && (m_registry.Replace(entity, Name{std::move(desc.common.name)}));
		const bool bWritten = bWrittencommon && (WriteVisibility(entity, desc.common.bVisible));

		if (bWritten)
		{
			RecordChange(entity);
		}
		return bWritten;
	}

}
