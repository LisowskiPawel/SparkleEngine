#include "PCH.h"

#include "Metadata/CookedSceneMetadataBuilder.h"

#include "GameFramework/Public/Assets/Cooked/CookedSceneMetadataRecords.h"
#include "SourceImportOutput.h"

#include <algorithm>

std::uint32_t CookedSceneMetadataBuilder::BuildFeatureFlags(const SourceImportOutput& importOutput) noexcept
{
	using Assets::CookedSceneFeatureFlags;
	using Assets::ToCookedSceneFeatureFlagMask;

	std::uint32_t flags = 0;
	if (!importOutput.scene.cameras.empty())
	{
		flags |= ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Cameras);
	}
	if (!importOutput.scene.lights.empty())
	{
		flags |= ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Lights);
	}
	if (!importOutput.scene.animations.empty())
	{
		flags |= ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Animations);
	}
	if (!importOutput.scene.materialVariants.empty() || !importOutput.scene.materialVariantMappings.empty())
	{
		flags |= ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::MaterialVariants);
	}

	bool bHasSkinnedMeshes = !importOutput.scene.skeletons.empty();
	for (const ImportedMeshPrimitive& primitive : importOutput.scene.meshPrimitives)
	{
		if (primitive.geometry.HasSkinInfluences())
		{
			bHasSkinnedMeshes = true;
			if (primitive.geometry.HasMorphTargets())
			{
				flags |= ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::MorphTargets);
			}
		}
	}
	if (bHasSkinnedMeshes)
	{
		flags |= ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::SkinnedMeshes) | ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::Skeletons);
	}

	const bool bHasAuthoredInstances = std::ranges::any_of(
	    importOutput.scene.meshInstanceGroups,
	    [](const ImportedMeshInstanceGroup& group) { return group.groupKind == ImportedMeshInstanceGroupKind::AuthoredInstanceGroup; });
	if (bHasAuthoredInstances)
	{
		flags |= ToCookedSceneFeatureFlagMask(CookedSceneFeatureFlags::AuthoredMeshInstancing);
	}
	return flags;
}
