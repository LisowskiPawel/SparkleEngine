#include "PCH.h"
#include "Scene/Model/EditorSceneModel.h"

#include "Scene/SceneObjectPresentation.h"

#include <algorithm>
#include <utility>

EditorSceneModel::EditorSceneModel(WorldReadView readView, WorldMaterialVariantView materialVariants) :
    m_readView(std::move(readView)),
    m_materialVariants(std::move(materialVariants))
{
	RebuildEntries();
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

bool EditorSceneModel::Contains(const SceneObjectSelection& selection) const noexcept
{
	switch (selection.type)
	{
		case SceneObjectType::Camera:
			return FindCamera(selection.entity) != nullptr;
		case SceneObjectType::Light:
			return FindLight(selection.entity) != nullptr;
		case SceneObjectType::Mesh:
			return FindMesh(selection.entity) != nullptr;
		case SceneObjectType::Sky:
			return true;
		case SceneObjectType::None:
		default:
			return false;
	}
}

const EditorSceneEntry* EditorSceneModel::FindEntry(const SceneObjectSelection& selection) const noexcept
{
	const auto find = [&selection](const std::vector<EditorSceneEntry>& entries) -> const EditorSceneEntry*
	{
		const auto iterator = std::find_if(entries.begin(), entries.end(), [&selection](const EditorSceneEntry& entry) { return entry.Selection == selection; });
		return iterator == entries.end() ? nullptr : &*iterator;
	};

	switch (selection.type)
	{
		case SceneObjectType::Camera:
			return find(m_cameraEntries);
		case SceneObjectType::Light:
			return find(m_lightEntries);
		case SceneObjectType::Mesh:
			return find(m_meshEntries);
		case SceneObjectType::Sky:
			return &m_skyEntry;
		case SceneObjectType::None:
		default:
			return nullptr;
	}
}

void EditorSceneModel::RebuildEntries()
{
	const auto cameras = GetCameras();
	const auto lights = GetLights();
	const auto meshes = GetMeshes();
	const auto& sky = GetSkyEnvironment();
	m_cameraEntries.clear();
	m_lightEntries.clear();
	m_meshEntries.clear();
	m_cameraEntries.reserve(cameras.size());
	m_lightEntries.reserve(lights.size());
	m_meshEntries.reserve(meshes.size());
	for (std::size_t index = 0; index < cameras.size(); ++index)
	{
		const auto& camera = cameras[index];
		std::string label = camera.Name.empty() ? "Camera " + std::to_string(index + 1) : camera.Name;
		const SceneObjectSelection selection = SceneObjectSelection::Camera(camera.Entity);
		m_cameraEntries.push_back({std::move(label), "Camera", selection, camera.bVisible});
	}
	for (std::size_t index = 0; index < lights.size(); ++index)
	{
		const auto& light = lights[index];
		std::string label = SceneObjectPresentation::BuildLightLabel(light.Description, index);
		const SceneLightKind kind = light.Description.GetKind();
		const char* typeLabel = SceneObjectPresentation::GetLightTypeLabel(kind);
		const SceneObjectSelection selection = SceneObjectSelection::Light(light.Entity);
		m_lightEntries.push_back({std::move(label), typeLabel, selection, light.Description.common.bVisible, kind});
	}
	for (std::size_t index = 0; index < meshes.size(); ++index)
	{
		const auto& mesh = meshes[index];
		std::string label = "Mesh " + std::to_string(index + 1);
		const char* typeLabel = mesh.Kind == SceneMeshKind::Skeletal ? "Skeletal Mesh" : "Static Mesh";
		const SceneObjectSelection selection = SceneObjectSelection::Mesh(mesh.Entity);
		m_meshEntries.push_back({std::move(label), typeLabel, selection, mesh.bVisible});
	}
	m_skyEntry = {sky ? "Sky" : "Sky (Engine Default)", "Sky", SceneObjectSelection::Sky(), !sky || sky->bEnabled};
}
