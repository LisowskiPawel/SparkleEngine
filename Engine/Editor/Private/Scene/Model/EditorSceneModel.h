#pragma once

#include "Scene/SceneObjectSelection.h"
#include "World/WorldMaterialVariantView.h"
#include "World/WorldReadView.h"

#include <cstdint>
#include <optional>
#include <span>

// Retains the immutable world snapshot used for this editor frame; no derived entry cache.
class EditorSceneModel final
{
public:
	EditorSceneModel() = default;
	EditorSceneModel(WorldReadView readView, WorldMaterialVariantView materialVariants);

	bool HasWorld() const noexcept { return m_readView.IsValid(); }

	WorldSequence GetWorldSequence() const noexcept { return m_readView.GetSequence(); }

	std::uint64_t GetWorldGeneration() const noexcept { return m_readView.GetGeneration(); }

	std::span<const WorldCameraReadData> GetCameras() const noexcept { return m_readView.GetCameras(); }

	std::span<const WorldLightReadData> GetLights() const noexcept { return m_readView.GetLights(); }

	std::span<const WorldMeshReadData> GetMeshes() const noexcept { return m_readView.GetMeshes(); }

	const std::optional<SkyEnvironment>& GetSkyEnvironment() const noexcept { return m_readView.GetSkyEnvironment(); }

	const WorldMaterialVariantView& GetMaterialVariants() const noexcept { return m_materialVariants; }

	const WorldCameraReadData* FindCamera(EntityId entity) const noexcept;
	const WorldLightReadData* FindLight(EntityId entity) const noexcept;
	const WorldMeshReadData* FindMesh(EntityId entity) const noexcept;
	std::optional<bool> ReadVisibility(const SceneObjectSelection& selection) const noexcept;
	bool Contains(const SceneObjectSelection& selection) const noexcept;

private:
	WorldReadView m_readView;
	WorldMaterialVariantView m_materialVariants;
};
