#pragma once

#include <cstdint>
#include "World/EntityId.h"

enum class SceneObjectType : std::uint8_t
{
	None = 0,
	Camera,
	Sky,
	Light,
	Mesh
};

struct SceneObjectSelection final
{
	SceneObjectType type = SceneObjectType::None;
	EntityId entity;

	static SceneObjectSelection None() noexcept;
	static SceneObjectSelection Camera(EntityId entity) noexcept;
	static SceneObjectSelection Sky() noexcept;
	static SceneObjectSelection Light(EntityId entity) noexcept;
	static SceneObjectSelection Mesh(EntityId entity) noexcept;

	bool IsNone() const noexcept;

	bool operator==(const SceneObjectSelection& other) const noexcept;
};
