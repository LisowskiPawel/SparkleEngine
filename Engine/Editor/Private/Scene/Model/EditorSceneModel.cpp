#include "PCH.h"

#include "Scene/Model/EditorSceneModel.h"

#include <algorithm>
#include <utility>

EditorSceneModel::EditorSceneModel(WorldReadView readView, WorldMaterialVariantView materialVariants) :
    m_readView(std::move(readView)),
    m_materialVariants(std::move(materialVariants))
{
}

template <typename T> static const T* FindByEntity(std::span<const T> values, EntityId entity) noexcept
{
	const auto iterator = std::lower_bound(values.begin(), values.end(), entity, [](const T& value, EntityId key) { return value.Entity < key; });
	return iterator != values.end() && iterator->Entity == entity ? &*iterator : nullptr;
}

const WorldCameraReadData* EditorSceneModel::FindCamera(EntityId entity) const noexcept
{
	return FindByEntity(GetCameras(), entity);
}

const WorldLightReadData* EditorSceneModel::FindLight(EntityId entity) const noexcept
{
	return FindByEntity(GetLights(), entity);
}

const WorldMeshReadData* EditorSceneModel::FindMesh(EntityId entity) const noexcept
{
	return FindByEntity(GetMeshes(), entity);
}

std::optional<bool> EditorSceneModel::ReadVisibility(const SceneObjectSelection& selection) const noexcept
{
	if (!HasWorld())
	{
		return std::nullopt;
	}
	if (selection.type == SceneObjectType::Camera)
	{
		const auto* camera = FindCamera(selection.entity);
		return camera ? std::optional(camera->bVisible) : std::nullopt;
	}
	if (selection.type == SceneObjectType::Light)
	{
		const auto* light = FindLight(selection.entity);
		return light ? std::optional(light->Description.common.bVisible) : std::nullopt;
	}
	if (selection.type == SceneObjectType::Mesh)
	{
		const auto* mesh = FindMesh(selection.entity);
		return mesh ? std::optional(mesh->bVisible) : std::nullopt;
	}
	if (selection.type == SceneObjectType::Sky)
	{
		const auto& sky = GetSkyEnvironment();
		return !sky || sky->bEnabled;
	}
	return std::nullopt;
}

bool EditorSceneModel::Contains(const SceneObjectSelection& selection) const noexcept
{
	return ReadVisibility(selection).has_value();
}
