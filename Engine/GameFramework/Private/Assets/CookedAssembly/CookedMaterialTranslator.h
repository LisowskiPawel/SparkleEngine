#pragma once

#include "Assets/Cooked/LoadedMaterialAsset.h"
#include "Scene/Materials/MaterialDesc.h"

namespace Assets
{
	namespace CookedMaterialTranslator
	{
		MaterialDesc Translate(const LoadedMaterialAsset& materialAsset);
	}
}
