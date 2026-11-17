#include "PCH.h"
#include "Scene/SceneObjectSelection.h"

SceneObjectSelection SceneObjectSelection::None() noexcept
{
	return {};
}

SceneObjectSelection SceneObjectSelection::Camera(EntityId entity) noexcept
{
	return {SceneObjectType::Camera, entity};
}

SceneObjectSelection SceneObjectSelection::Sky() noexcept
{
	return {SceneObjectType::Sky, EntityId::Invalid()};
}

SceneObjectSelection SceneObjectSelection::Light(EntityId entity) noexcept
{
	return {SceneObjectType::Light, entity};
}

SceneObjectSelection SceneObjectSelection::Mesh(EntityId entity) noexcept
{
	return {SceneObjectType::Mesh, entity};
}

bool SceneObjectSelection::IsNone() const noexcept
{
	return type == SceneObjectType::None;
}

bool SceneObjectSelection::operator==(const SceneObjectSelection& other) const noexcept
{
	return type == other.type && entity == other.entity;
}
