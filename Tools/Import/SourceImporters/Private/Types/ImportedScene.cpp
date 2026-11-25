#include "PCH.h"

#include "Types/ImportedScene.h"

#include <limits>

ImportedSkeletonIndex ImportedScene::FindSkeletonForSourceSkin(std::uint32_t sourceSkinIndex) const noexcept
{
	for (std::size_t skeletonIndex = 0; skeletonIndex < skeletons.size(); ++skeletonIndex)
	{
		if (skeletons[skeletonIndex].sourceSkinIndex == sourceSkinIndex)
		{
			return static_cast<ImportedSkeletonIndex>(skeletonIndex);
		}
	}
	return kInvalidImportedSkeletonIndex;
}

std::pair<ImportedSkeletonIndex, std::uint32_t> ImportedScene::FindSkeletonJointForNode(std::uint32_t sourceNodeIndex) const noexcept
{
	for (std::size_t skeletonIndex = 0; skeletonIndex < skeletons.size(); ++skeletonIndex)
	{
		const ImportedSkeleton& skeleton = skeletons[skeletonIndex];
		for (std::size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex)
		{
			if (skeleton.joints[jointIndex].sourceNodeIndex == sourceNodeIndex)
			{
				return {static_cast<std::uint32_t>(skeletonIndex), static_cast<std::uint32_t>(jointIndex)};
			}
		}
	}

	return {(std::numeric_limits<std::uint32_t>::max)(), (std::numeric_limits<std::uint32_t>::max)()};
}

bool ImportedScene::HasSupportedContent() const noexcept
{
	if (meshPrimitives.empty() != meshInstances.empty())
	{
		return false;
	}

	const bool bGeometryOrCamera = !meshPrimitives.empty() || !cameras.empty();
	const bool bLightOrAnimation = !lights.empty() || !animations.empty();
	return bGeometryOrCamera || bLightOrAnimation;
}
