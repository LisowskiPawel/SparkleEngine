#pragma once

#include "ImportedCamera.h"
#include "ImportedAnimation.h"
#include "ImportedGeometry.h"
#include "ImportedLight.h"
#include "ImportedMaterial.h"
#include "ImportedMaterialVariant.h"
#include "ImportedSkin.h"

#include <vector>
#include <utility>

struct ImportedScene
{
	std::vector<ImportedMeshPrimitive> meshPrimitives;
	std::vector<ImportedMeshInstance> meshInstances;
	std::vector<ImportedMeshInstanceGroup> meshInstanceGroups;
	std::vector<ImportedCamera> cameras;
	std::vector<ImportedLight> lights;
	std::vector<ImportedAnimationClip> animations;
	std::vector<ImportedMaterial> materials;
	std::vector<ImportedMaterialVariant> materialVariants;
	std::vector<ImportedMaterialVariantMapping> materialVariantMappings;
	std::vector<ImportedSkeleton> skeletons;

	bool HasSupportedContent() const noexcept;

	ImportedSkeletonIndex FindSkeletonForSourceSkin(std::uint32_t sourceSkinIndex) const noexcept;
	std::pair<ImportedSkeletonIndex, std::uint32_t> FindSkeletonJointForNode(std::uint32_t sourceNodeIndex) const noexcept;
};
