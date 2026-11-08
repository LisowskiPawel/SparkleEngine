#include "PCH.h"
#include "World/GameWorldState.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"
#include "World/WorldTransformConversion.h"

namespace ECS
{
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
