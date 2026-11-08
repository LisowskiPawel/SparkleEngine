#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"
#include "World/Publication/WorldReadViewStorage.h"

#include <algorithm>
#include <utility>

template <typename T> static void EraseReadViewEntity(std::vector<T>& values, EntityId entity)
{
	const auto position = std::lower_bound(values.begin(), values.end(), entity, [](const T& value, EntityId key) { return value.Entity < key; });
	if (position != values.end() && position->Entity == entity)
	{
		values.erase(position);
	}
}

template <typename T> static void UpsertReadViewEntity(std::vector<T>& values, T value)
{
	auto position = std::lower_bound(values.begin(), values.end(), value.Entity, [](const T& existing, EntityId entity) { return existing.Entity < entity; });
	if (position != values.end() && position->Entity == value.Entity)
	{
		*position = std::move(value);
	}
	else
	{
		values.insert(position, std::move(value));
	}
}

namespace ECS
{
	WorldCameraReadData GameWorldState::BuildCameraReadData(EntityId entity) const
	{
		WorldCameraReadData read;
		read.Entity = entity;
		if (const std::optional<SceneCameraEntry> entry = ReadCamera(entity))
		{
			read.Name = entry->name;
			read.Description = entry->desc;
		}
		read.LocalTransform = ReadTransform(entity);
		if (const CameraDerivedState* derived = m_registry.Get<CameraDerivedState>(entity))
		{
			read.Direction = derived->Direction;
		}
		if (const Camera* camera = m_registry.Get<Camera>(entity))
		{
			read.AspectRatio = camera->AspectRatio;
			read.bActive = camera->bActive;
		}
		read.bVisible = ReadVisibility(entity);
		return read;
	}

	WorldLightReadData GameWorldState::BuildLightReadData(EntityId entity) const
	{
		WorldLightReadData read;
		read.Entity = entity;
		if (std::optional<SceneLightDesc> light = ReadLight(entity))
		{
			read.Description = std::move(*light);
		}
		return read;
	}

	WorldMeshReadData GameWorldState::BuildMeshReadData(EntityId entity) const
	{
		WorldMeshReadData read;
		read.Entity = entity;
		read.LocalTransform = ReadTransform(entity);
		if (const MeshInstance* mesh = m_registry.Get<MeshInstance>(entity))
		{
			read.Material = mesh->Material;
			read.MeshAssetId = mesh->MeshAssetId;
			read.Kind = mesh->Kind;
		}
		read.bVisible = ReadVisibility(entity);
		return read;
	}

	void GameWorldState::BuildReadViewBaseline(WorldReadView::Storage& next) const
	{
		for (EntityId entity : m_registry.GetStorage<Camera>().GetEntities())
		{
			next.Cameras.push_back(BuildCameraReadData(entity));
		}
		for (EntityId entity : m_registry.GetStorage<Light>().GetEntities())
		{
			next.Lights.push_back(BuildLightReadData(entity));
		}
		for (EntityId entity : m_registry.GetStorage<MeshInstance>().GetEntities())
		{
			next.Meshes.push_back(BuildMeshReadData(entity));
		}
		std::sort(next.Cameras.begin(), next.Cameras.end(), [](const auto& a, const auto& b) { return a.Entity < b.Entity; });
		std::sort(next.Lights.begin(), next.Lights.end(), [](const auto& a, const auto& b) { return a.Entity < b.Entity; });
		std::sort(next.Meshes.begin(), next.Meshes.end(), [](const auto& a, const auto& b) { return a.Entity < b.Entity; });
	}

	void GameWorldState::RefreshReadViewEntity(WorldReadView::Storage& next, EntityId entity) const
	{
		if (m_registry.Get<Camera>(entity) != nullptr)
		{
			UpsertReadViewEntity(next.Cameras, BuildCameraReadData(entity));
		}
		else
		{
			EraseReadViewEntity(next.Cameras, entity);
		}
		if (m_registry.Get<Light>(entity) != nullptr)
		{
			UpsertReadViewEntity(next.Lights, BuildLightReadData(entity));
		}
		else
		{
			EraseReadViewEntity(next.Lights, entity);
		}
		if (m_registry.Get<MeshInstance>(entity) != nullptr)
		{
			UpsertReadViewEntity(next.Meshes, BuildMeshReadData(entity));
		}
		else
		{
			EraseReadViewEntity(next.Meshes, entity);
		}
	}

	void GameWorldState::PublishReadView(std::span<const EntityId> changes, bool bFullBaseline)
	{
		const std::shared_ptr<const WorldReadView::Storage> previous = m_publishedReadView.load(std::memory_order_acquire);
		auto next = previous != nullptr && !bFullBaseline ? std::make_shared<WorldReadView::Storage>(*previous) : std::make_shared<WorldReadView::Storage>();
		next->Sequence = previous != nullptr ? previous->Sequence + 1 : 1;

		if (previous == nullptr || bFullBaseline)
		{
			BuildReadViewBaseline(*next);
		}
		else
		{
			for (EntityId entity : changes)
			{
				RefreshReadViewEntity(*next, entity);
			}
		}
		next->Sky = m_skyEnvironment;
		m_publishedReadView.store(std::move(next), std::memory_order_release);
	}

	void GameWorldState::CommitExtraction()
	{
		m_extraction.CommitMeshes(m_meshInstanceGroups);
		PublishPendingChanges();
	}
}
