#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"
#include "World/WorldTransformConversion.h"

#include <algorithm>

class CameraComponentTranslation final
{
public:
	static ECS::Camera ToCameraData(const CameraDesc& desc, float aspectRatio, bool bActive) noexcept
	{
		return ECS::Camera{
		    .FovYDegrees = std::clamp(desc.fovYDegrees, 1.0f, 179.0f),
		    .NearZ = desc.nearZ,
		    .FarZ = desc.farZ,
		    .AspectRatio = aspectRatio,
		    .ProjectionKind = desc.projectionKind,
		    .bActive = bActive};
	}
};

namespace ECS
{
	EntityId GameWorldState::AddCamera(SceneCameraEntry&& entry, bool bActive)
	{
		const EntityId entity = m_registry.Create();
		if (!entity.IsValid())
		{
			return entity;
		}

		Transform transform(entry.desc.position, {entry.desc.pitchRadians, entry.desc.yawRadians, 0.0f});
		const LocalTransform local = WorldTransformConversion::ToLocal(transform);

		const bool bTransformsAdded = m_registry.Add(entity, local) && m_registry.Add(entity, WorldTransform{});
		const bool bCameraAdded = bTransformsAdded && (m_registry.Add(entity, CameraComponentTranslation::ToCameraData(entry.desc, 16.0f / 9.0f, bActive)));
		const bool bDerivedStateAdded = bCameraAdded && (m_registry.Add(entity, CameraDerivedState{}));
		const bool bVisibilityAdded = bDerivedStateAdded && (m_registry.Add(entity, Visibility{}));
		const bool bNameAdded = bVisibilityAdded && (m_registry.Add(entity, Name{std::move(entry.name)}));
		const bool bIdentityAdded = bNameAdded && (m_registry.Add(entity, AuthoredIdentity{.SourceObjectId = ++m_nextCameraIdentity, .Kind = AuthoredObjectKind::Camera}));
		const bool bAdded = bIdentityAdded && (m_registry.Add(entity, EditorMetadata{}));

		if (!bAdded)
		{
			m_registry.Destroy(entity);
			return EntityId::Invalid();
		}
		MarkTransformDirty(entity);
		RecordChange(entity);
		if (bActive || !m_activeCamera.IsValid())
		{
			SetActiveCamera(entity);
		}
		return entity;
	}

	std::size_t GameWorldState::GetCameraCount() const noexcept
	{
		return Count<Camera>();
	}

	EntityId GameWorldState::GetCameraEntity(std::size_t index) const noexcept
	{
		return EntityAt<Camera>(index);
	}

	bool GameWorldState::IsCamera(EntityId entity) const noexcept
	{
		return m_registry.Get<Camera>(entity) != nullptr;
	}

	std::optional<SceneCameraEntry> GameWorldState::ReadCamera(EntityId entity) const
	{
		const Camera* camera = m_registry.Get<Camera>(entity);
		const LocalTransform* local = m_registry.Get<LocalTransform>(entity);
		if (camera == nullptr || local == nullptr)
		{
			return std::nullopt;
		}
		SceneCameraEntry entry;
		if (const Name* name = m_registry.Get<Name>(entity))
		{
			entry.name = name->Value;
		}
		const Transform transform = WorldTransformConversion::ToPublic(*local);
		const DirectX::XMFLOAT3 rotation = transform.GetRotationEuler();
		entry.desc.position = transform.GetTranslation();
		entry.desc.pitchRadians = rotation.x;
		entry.desc.yawRadians = rotation.y;
		entry.desc.fovYDegrees = camera->FovYDegrees;
		entry.desc.nearZ = camera->NearZ;
		entry.desc.farZ = camera->FarZ;
		entry.desc.projectionKind = camera->ProjectionKind;
		return entry;
	}

	bool GameWorldState::SetActiveCamera(EntityId entity) noexcept
	{
		const Camera* selected = m_registry.Get<Camera>(entity);
		if (selected == nullptr || selected->ProjectionKind != CameraProjectionKind::Perspective)
		{
			return false;
		}
		if (m_activeCamera.IsValid() && m_activeCamera != entity)
		{
			if (const Camera* previous = m_registry.Get<Camera>(m_activeCamera))
			{
				Camera inactive = *previous;
				inactive.bActive = false;
				m_registry.Replace(m_activeCamera, inactive);
				RecordChange(m_activeCamera);
			}
		}
		Camera active = *selected;
		active.bActive = true;
		if (!m_registry.Replace(entity, active))
		{
			return false;
		}
		m_activeCamera = entity;
		RecordChange(entity);
		return true;
	}

	bool GameWorldState::WriteCameraDesc(EntityId entity, const CameraDesc& desc) noexcept
	{
		const Camera* existing = m_registry.Get<Camera>(entity);
		if (existing == nullptr)
		{
			return false;
		}
		Transform transform(desc.position, {desc.pitchRadians, desc.yawRadians, 0.0f});
		const bool bWritten = WriteTransform(entity, transform) && m_registry.Replace(entity, CameraComponentTranslation::ToCameraData(desc, existing->AspectRatio, existing->bActive));
		if (bWritten)
		{
			RecordChange(entity);
		}
		return bWritten;
	}

	bool GameWorldState::WriteTransform(EntityId entity, const Transform& transform) noexcept
	{
		const bool bWritten = m_registry.Replace(entity, WorldTransformConversion::ToLocal(transform));
		if (bWritten)
		{
			MarkTransformDirty(entity);
			RecordChange(entity);
		}
		return bWritten;
	}

	Transform GameWorldState::ReadTransform(EntityId entity) const noexcept
	{
		const LocalTransform* local = m_registry.Get<LocalTransform>(entity);
		return local == nullptr ? Transform{} : WorldTransformConversion::ToPublic(*local);
	}

	bool GameWorldState::WriteVisibility(EntityId entity, bool bVisible) noexcept
	{
		const Visibility* visibility = m_registry.Get<Visibility>(entity);
		if (visibility == nullptr)
		{
			return false;
		}
		Visibility updated = *visibility;
		updated.bVisible = bVisible;
		const bool bWritten = m_registry.Replace(entity, updated);
		if (bWritten)
		{
			RecordChange(entity);
		}
		return bWritten;
	}

	bool GameWorldState::ReadVisibility(EntityId entity) const noexcept
	{
		const Visibility* visibility = m_registry.Get<Visibility>(entity);
		return visibility != nullptr && visibility->bVisible;
	}

}
