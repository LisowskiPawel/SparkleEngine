#pragma once

#include "Assets/Cooked/CookedMaterialAsset.h"
#include "GameFramework/Public/Assets/Cooked/CookedTextureReference.h"

#include <string>
#include <vector>

namespace Assets
{
	struct LoadedMaterialAsset
	{
		CookedMaterialAssetHeader header;
		std::string name;
		std::vector<CookedTextureReference> textureReferences;
	};
}
