#pragma once

#include "Assets/SceneAssetPayload.h"
#include "Scene/Meshes/MeshInstanceGroup.h"
#include "World/Resources/MaterialVariantResourceStore.h"

#include <vector>
#include <span>

namespace SceneMaterialVariantTranslator
{
	std::vector<MaterialVariantDesc> BuildDescriptions(const SceneAssetPayload& sceneAssetPayload);

	std::vector<MaterialVariantBinding> BuildBindings(const SceneAssetPayload& sceneAssetPayload, MaterialHandle materialBaseHandle, std::span<const EntityId> entities);
}
