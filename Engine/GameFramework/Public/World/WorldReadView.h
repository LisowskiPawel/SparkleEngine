#pragma once

#include "GameFramework/Public/Assets/Cooked/CookedAssetCommon.h"
#include "GameFramework/Public/GameFrameworkAPI.h"
#include "GameFramework/Public/Scene/Camera/CameraDesc.h"
#include "GameFramework/Public/Scene/Lighting/SceneLightDesc.h"
#include "GameFramework/Public/Scene/Materials/MaterialHandle.h"
#include "GameFramework/Public/Scene/Meshes/SceneMeshKind.h"
#include "GameFramework/Public/Scene/Transform.h"
#include "GameFramework/Public/World/EntityId.h"
#include "GameFramework/Public/World/SkyEnvironment.h"

#include <cstdint>
#include <DirectXMath.h>
#include <memory>
#include <optional>
#include <span>
#include <string>

using WorldSequence = std::uint64_t;

namespace ECS
{
	class GameWorldState;
}

struct WorldCameraReadData final
{
	EntityId Entity;
	std::string Name;
	CameraDesc Description;
	Transform LocalTransform;
	DirectX::XMFLOAT3 Direction{0.0f, 0.0f, 1.0f};
	float AspectRatio = 1.0f;
	bool bVisible = true;
	bool bActive = false;
};

struct WorldLightReadData final
{
	EntityId Entity;
	SceneLightDesc Description;
};

struct WorldMeshReadData final
{
	EntityId Entity;
	Transform LocalTransform;
	MaterialHandle Material = MaterialHandle::Invalid();
	Assets::CookedAssetId MeshAssetId = Assets::InvalidCookedAssetId;
	SceneMeshKind Kind = SceneMeshKind::Static;
	bool bVisible = true;
};

class SPARKLE_ENGINE_API WorldReadView final
{
public:
	WorldReadView() noexcept = default;

	bool IsValid() const noexcept { return m_storage != nullptr; }

	std::uint64_t GetGeneration() const noexcept;
	WorldSequence GetSequence() const noexcept;
	std::span<const WorldCameraReadData> GetCameras() const noexcept;
	std::span<const WorldLightReadData> GetLights() const noexcept;
	std::span<const WorldMeshReadData> GetMeshes() const noexcept;
	const std::optional<SkyEnvironment>& GetSkyEnvironment() const noexcept;

private:
	struct Storage;
	friend class ECS::GameWorldState;

	WorldReadView(std::shared_ptr<const Storage> storage, std::uint64_t generation) noexcept;

	std::shared_ptr<const Storage> m_storage;
	std::uint64_t m_generation = 0;
};
