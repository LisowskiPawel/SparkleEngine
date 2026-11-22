#include "PCH.h"

#include "Types/FindImportedSkeletonForSourceSkin.h"
#include "Types/ImportedScene.h"

ImportedSkeletonIndex FindImportedSkeletonForSourceSkin(const ImportedScene& scene, std::uint32_t sourceSkinIndex) noexcept
{
	for (std::size_t skeletonIndex = 0; skeletonIndex < scene.skeletons.size(); ++skeletonIndex)
	{
		if (scene.skeletons[skeletonIndex].sourceSkinIndex == sourceSkinIndex)
		{
			return static_cast<ImportedSkeletonIndex>(skeletonIndex);
		}
	}
	return kInvalidImportedSkeletonIndex;
}
