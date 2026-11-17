#pragma once

#include "Types/ImportedScene.h"

struct cgltf_data;

namespace GltfAnimationImporter
{
	void ImportAnimations(const cgltf_data& data, ImportedScene& importedScene);
}
