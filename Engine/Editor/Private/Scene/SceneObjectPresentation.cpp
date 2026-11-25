#include "PCH.h"
#include "Icons/EditorIcon.h"
#include "Scene/SceneObjectPresentation.h"

#include "Scene/Model/EditorSceneModel.h"
#include "Scene/SceneObjectSelection.h"
#include "Scene/Lighting/SceneLightDesc.h"

namespace SceneObjectPresentation
{
	std::string_view Entry::GetLabel() const noexcept
	{
		return std::visit([](const auto& label) -> std::string_view { return label; }, Label);
	}

	Entry BuildEntry(const WorldCameraReadData& camera, std::size_t index)
	{
		Entry entry{std::string_view(camera.Name), "Camera", SceneObjectSelection::Camera(camera.Entity), camera.bVisible};
		if (camera.Name.empty())
		{
			entry.Label = "Camera " + std::to_string(index + 1);
		}
		return entry;
	}

	Entry BuildEntry(const WorldLightReadData& light, std::size_t index)
	{
		const SceneLightKind kind = light.Description.GetKind();
		const char* typeLabel = GetLightTypeLabel(kind);
		Entry entry{std::string_view(light.Description.common.name), typeLabel, SceneObjectSelection::Light(light.Entity), light.Description.common.bVisible, kind};
		if (light.Description.common.name.empty())
		{
			entry.Label = std::string(typeLabel) + " " + std::to_string(index + 1);
		}
		return entry;
	}

	Entry BuildEntry(const WorldMeshReadData& mesh, std::size_t index)
	{
		const char* typeLabel = mesh.Kind == SceneMeshKind::Skeletal ? "Skeletal Mesh" : "Static Mesh";
		return {"Mesh " + std::to_string(index + 1), typeLabel, SceneObjectSelection::Mesh(mesh.Entity), mesh.bVisible};
	}

	Entry BuildEntry(const std::optional<SkyEnvironment>& sky, std::size_t)
	{
		return {std::string_view(sky ? "Sky" : "Sky (Engine Default)"), "Sky", SceneObjectSelection::Sky(), !sky || sky->bEnabled};
	}

	std::optional<Entry> BuildEntry(const EditorSceneModel& scene, const SceneObjectSelection& selection)
	{
		if (selection.type == SceneObjectType::Camera)
		{
			const auto* camera = scene.FindCamera(selection.entity);
			if (camera == nullptr)
			{
				return std::nullopt;
			}
			const auto index = static_cast<std::size_t>(camera - scene.GetCameras().data());
			return BuildEntry(*camera, index);
		}
		if (selection.type == SceneObjectType::Light)
		{
			const auto* light = scene.FindLight(selection.entity);
			if (light == nullptr)
			{
				return std::nullopt;
			}
			const auto index = static_cast<std::size_t>(light - scene.GetLights().data());
			return BuildEntry(*light, index);
		}
		if (selection.type == SceneObjectType::Mesh)
		{
			const auto* mesh = scene.FindMesh(selection.entity);
			if (mesh == nullptr)
			{
				return std::nullopt;
			}
			const auto index = static_cast<std::size_t>(mesh - scene.GetMeshes().data());
			return BuildEntry(*mesh, index);
		}
		return selection.type == SceneObjectType::Sky ? std::optional(BuildEntry(scene.GetSkyEnvironment(), 0)) : std::nullopt;
	}

	EditorIcons::EditorIcon GetLightIcon(SceneLightKind kind) noexcept
	{
		switch (kind)
		{
			case SceneLightKind::Directional:
				return EditorIcons::EditorIcon::DirectionalLight;
			case SceneLightKind::Point:
				return EditorIcons::EditorIcon::PointLight;
			case SceneLightKind::Spot:
				return EditorIcons::EditorIcon::SpotLight;
			case SceneLightKind::Rect:
				return EditorIcons::EditorIcon::Light;
			case SceneLightKind::Unknown:
			default:
				return EditorIcons::EditorIcon::Light;
		}
	}

	const char* GetLightTypeLabel(SceneLightKind kind) noexcept
	{
		switch (kind)
		{
			case SceneLightKind::Directional:
				return "Directional Light";
			case SceneLightKind::Point:
				return "Point Light";
			case SceneLightKind::Spot:
				return "Spot Light";
			case SceneLightKind::Rect:
				return "Rect Light";
			case SceneLightKind::Unknown:
			default:
				return "Light";
		}
	}

	EditorIcons::EditorIcon BuildSelectionIcon(const SceneObjectSelection& selection, SceneLightKind lightKind) noexcept
	{
		switch (selection.type)
		{
			case SceneObjectType::Camera:
				return EditorIcons::EditorIcon::Camera;
			case SceneObjectType::Sky:
				return EditorIcons::EditorIcon::ViewLit;
			case SceneObjectType::Light:
				return GetLightIcon(lightKind);
			case SceneObjectType::Mesh:
				return EditorIcons::EditorIcon::StaticMesh;
			case SceneObjectType::None:
			default:
				return EditorIcons::EditorIcon::None;
		}
	}
}
